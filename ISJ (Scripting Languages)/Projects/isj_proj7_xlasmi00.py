#!/usr/bin/env python3

import asyncio
import aiohttp


async def get_status(url:str) -> str:
    """
    Get status of given url
    ---------------
    Arguments:
    url:str
        url to get status from
    ---------------
    Return:
        url status
    """
    async with  aiohttp.ClientSession() as session:
        try:
            async with session.get(url) as response:
                return response.status
        except aiohttp.ClientError:
            return "aiohttp.ClientError"


async def get_urls(urls:list) -> list[tuple]:
    """
    Get stateses of every url in given list
    ---------------
    Arguments:
    urls:list
        list with urls to get status from
    ---------------
    Return:
        List with tuples (status, url)
    """
    tasks = []
    for url in urls:
        task = asyncio.create_task(get_status(url))
        tasks.append(task)
    results = await asyncio.gather(*tasks)
    return [(result, url) for result, url in zip(results, urls)]


if __name__ == '__main__':

    urls = ['https://www.fit.vutbr.cz', 'https://www.szn.cz', 'https://www.alza.cz', 'https://office.com', 'https://aukro.cz']

    # for MS Windows

    # asyncio.set_event_loop_policy(asyncio.WindowsSelectorEventLoopPolicy())

    res = asyncio.run(get_urls(urls))

    print(res)