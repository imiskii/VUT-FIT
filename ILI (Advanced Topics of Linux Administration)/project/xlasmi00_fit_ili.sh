#!/bin/bash

# Author: Michal Ľaš (xlasmi00)
# Date: 17.10.2023


# execute with: ./xlasmi00_fit_ili.sh <repos> | less -R


# task-8: https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/deployment_guide/sec-setting_repository_options



# ------------------- Globals ------------------- #

# Colors 
REDCOLOR='\033[1;31m'
YELLOWCOLOR='\033[1;33m'
CYANCOLOR='\033[1;36m'
GREENCOLOR='\033[1;32m'
WHITECOLOR='\033[1;37m'
BLUECOLOR='\033[1;34m'
NOCOLOR='\033[0m'


# ------------------ Functions ------------------ #

printTasks()
{
    echo -e "${BLUECOLOR}taks-1: ${NOCOLOR}Create a 200 MB file /var/tmp/ukol.img"
    echo -e "${BLUECOLOR}taks-2: ${NOCOLOR}Create a loop device for ukol.img"
    echo -e "${BLUECOLOR}taks-3: ${NOCOLOR}Create a filesystem on the loop device from task-2 (ext4)"
    echo -e "${BLUECOLOR}taks-4: ${NOCOLOR}Modify /etc/fstab to automatically mount the filesystem (using the loop device)"
    echo -e "${BLUECOLOR}taks-5: ${NOCOLOR}Mount the filesystem from task-3 to /var/www/html/ukol"
    echo -e "${BLUECOLOR}taks-6: ${NOCOLOR}Using the yum tool, download packages from the system repositories provided as arguments to the script and save them to /var/www/html/ukol"
    echo -e "${BLUECOLOR}taks-7: ${NOCOLOR}Generate repodata in /var/www/html/ukol"
    echo -e "${BLUECOLOR}taks-7.1: ${NOCOLOR}IMPORTANT: Then, modify the SELinux context in /var/www/html/ukol using the command: restorecon -Rv /var/www/html/ukol"
    echo -e "${BLUECOLOR}taks-8: ${NOCOLOR}Configure /etc/yum.repos.d/ukol.repo to access the "ukol" repository via the URL: http://localhost/ukol"
    echo -e "${BLUECOLOR}taks-9: ${NOCOLOR}Install and start a web server using the httpd (Apache) service"
    echo -e "${BLUECOLOR}taks-10: ${NOCOLOR}List available yum repositories to verify that the "ukol" repository is accessible"
    echo -e "${BLUECOLOR}taks-11: ${NOCOLOR}Unmount the filesystem mounted to /var/www/html/ukol"
    echo -e "${BLUECOLOR}taks-12: ${NOCOLOR}Then, in the script, execute the command "mount '-a'" and verify that the filesystem on the loop device is mounted to /var/www/html/ukol"
    echo -e "${BLUECOLOR}taks-13: ${NOCOLOR}Use the yum command to display information (info) about available packages, disabling all repositories except the "ukol" repository"
    echo -e ""
}

# log <task number> <message> <message type>
log()
{
    local task="$1"
    local text="$2"
    local type="$3"
    local color=

    case "$type" in
        "error")
            color=$REDCOLOR
            ;;
        "warning")
            color=$YELLOWCOLOR
            ;;
        "info")
            color=$CYANCOLOR
            ;;
        "success")
            color=$GREENCOLOR
            ;;
        *)
            color=$WHITECOLOR
            ;;
    esac

    echo -e "\n${color}[${type}] ${BLUECOLOR}task-${task}: ${WHITECOLOR}${text}${NOCOLOR}\n"
}


# Check the exit status and log
checkLog()
{
    local task="$1"
    local suc="$2"
    local err="$3"

    if [ $? -eq 0 ]; then
        log "${task}" "${suc}" "success"
    else
        log "${task}" "${err}" "error"
    fi
}


# -------------------- Main --------------------- #

# Start with printing tasks
#printTasks


# Install required RPMs

log 0 "Installing required RPMs" "info"

log 0 "Installing Apache server" "info"
dnf install httpd -y
checkLog 0 "Apache server installed" "Apache server instalation failure"

log 0 "Installing createrepo" "info"
dnf install createrepo -y
checkLog 0 "createrepo installed" "createrepo instalation failure"


log 0 "Done: required RPMs installed" "success"


# TASK 1 - Create a 200 MB file /var/tmp/ukol.img

log 1 "Creating a 200 MB file /var/tmp/ukol.img" "info"

dd if=/dev/zero of=/var/tmp/ukol.img bs=1M count=200

checkLog 1 "200 MB file /var/tmp/ukol.img created" "error while creating 200 MB file /var/tmp/ukol.img"

# TASK 2 - Create a loop device for ukol.img

log 2 "Creating a loop device for ukol.img" "info"

losetup /dev/loop0 /var/tmp/ukol.img

checkLog 2 "loop device created" "error while creating loop device for ukol.img"

# TASK 3 - Create a filesystem on the loop device from task-2 (ext4)

log 3 "Creating a filesystem on the loop device /dev/loop0" "info"

mkfs.ext4 /dev/loop0

checkLog 3 "filesystem on /dev/loop0 created" "error while creating filesystem on the loop device /dev/loop0"

# TASK 4 - Modify /etc/fstab to automatically mount the filesystem (using the loop device)

log 5 "Creating directory ukol in /var/www/html" "info"

mkdir -p /var/www/html/ukol

checkLog 5 "directory ukol in /var/www/html created" "can not create directory ukol in /var/www/html"

log 4 "Modifying /etc/fstab" "info"

echo "/var/tmp/ukol.img /var/www/html/ukol ext4 loop 0 0" >> /etc/fstab

checkLog 4 "/etc/fstab modified" "can not modify /etc/fstab"

log 4 "Reloading systemctl daemon" "info"

systemctl daemon-reload

checkLog 4 "systemctl daemon reloaded" "error while reloading systemctl daemon"

# TASK 5 - Mount the filesystem from task-3 to /var/www/html/ukol

log 5 "Mounting filesystem to /var/www/html/ukol" "info"

mount /dev/loop0 /var/www/html/ukol

checkLog 5 "filesystem was mounted" "error while mounting filesystem to /var/www/html/ukol"

# TASK 6 - Using the yum tool, download packages from the system repositories provided as arguments to the script and save them to /var/www/html/ukol

log 6 "downloading packages from provided arguments to /var/www/html/ukol" "info"

yum install -y $@ --downloadonly --downloaddir=/var/www/html/ukol

checkLog 6 "packages downloaded" "error while downloading packages"

# TASK 7 - Generate repodata in /var/www/html/ukol

log 7 "Generating repodata in /var/html/www/ukol" "info"

createrepo /var/www/html/ukol

checkLog 7 "Repodata generated" "error while generating repodata"

# TASK 7.1 - IMPORTANT: Then, modify the SELinux context in /var/www/html/ukol using the command: restorecon -Rv /var/www/html/ukol

log 7 "Modifying SELinux context in /var/www/html/ukol" "info"

restorecon -Rv /var/www/html/ukol

checkLog 7 "SeLinux context in /var/www/html/ukol modified" "error while modifying SELinux context in /var/www/html/ukol"

# TASK 8 - Configure /etc/yum.repos.d/ukol.repo to access the ukol repository via the URL: http://localhost/ukol

log 8 "Configuring /etc/yum.repos.d/ukol.repo" "info"

echo -e "[ukol]\nname=Ukol Repository\nbaseurl=http://localhost/ukol\ngpgcheck=0\nenabled=1" >> /etc/yum.repos.d/ukol.repo

checkLog 8 "/etc/yum.repos.d/ukol.repo created" "error while creating /etc/yum.repos.d/ukol.repo"

# TASK 9 - Install and start a web server using the httpd (Apache) service

log 9 "Starting Apache service" "info"

systemctl start httpd

checkLog 9 "Apache service started" "error while starting Apache service"

# TASK 10 - List available yum repositories to verify that the ukol repository is accessible

log 10 "Showing available yum repositories" "info"

yum repolist

log 10 "Done" "success"

# TASK 11 - Unmount the filesystem mounted to /var/www/html/ukol

log 11 "Unmounting /var/www/html/ukol" "info"

umount /var/www/html/ukol

checkLog 11 "/var/www/html/ukol unmounted" "error while unmounting /var/www/html/ukol"

log 11 "Showing mounted filesystems" "info"

df -h

# TASK 12 - Then, in the script, execute the command mount -a and verify that the filesystem on the loop device is mounted to /var/www/html/ukol

log 12 "Mounting all filesystems in /etc/fstab" "info"

mount -a

checkLog 12 "filesystems in /etc/fstab mounted" "error while mounting filesystems in /etc/fstab"

log 12 "Showing mounted filesystems" "info"

df -h

# TASK 13 - Use the yum command to display information (info) about available packages, disabling all repositories except the ukol repository

log 13 "Showing information about available packages" "info"

yum --disablerepo=* --enablerepo=ukol list available

log 13 "Done" "success"

# END OF FILE