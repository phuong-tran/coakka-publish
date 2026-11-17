"""Replay HTTP/2 upload reset, sibling completion, and upload on a new TCP socket."""

import argparse
import asyncio
from pathlib import Path

from lifecycle_interactions_wire import MULTIPART, MULTIPART_HEADERS, h2_peer

PREFIX_SIZE = 32


async def exercise(options):
    application = await asyncio.create_subprocess_exec(
        options.consumer, options.mode, options.certificate, options.key, options.backend,
        stdin=asyncio.subprocess.PIPE, stdout=asyncio.subprocess.PIPE,
    )

    async def marker(expected):
        line = (await asyncio.wait_for(application.stdout.readline(), 6)).decode().strip()
        if line != expected and not (expected == "READY " and line.startswith(expected)):
            raise RuntimeError(f"Expected fixture marker {expected!r}, got {line!r}")
        print(line, flush=True)
        return line

    try:
        port = int((await marker("READY ")).split()[1])
        async with h2_peer(port, options.ca) as peer:
            original = await peer.request(b"/upload", b"POST", len(MULTIPART), MULTIPART_HEADERS)
            await peer.send_data(original, MULTIPART[:PREFIX_SIZE])
            await marker("FIRST-HEAD")
            await marker("FIRST-DATA-CONSUMED" if options.mode == "service" else "FIRST-DATA-RETURNED")
            await peer.reset(original)
            sibling = await peer.request(b"/sibling")
            await peer.verify(sibling, b"ready")
            await marker("FIRST-BODY-CANCELLED" if options.mode == "service" else "FIRST-TERMINAL")

        # A new socket is part of the failing order. The old client transport
        # can finish retirement asynchronously after the sibling reply.
        async with h2_peer(port, options.ca) as peer:
            fresh = await peer.request(b"/upload", b"POST", len(MULTIPART), MULTIPART_HEADERS)
            await peer.send_data(fresh, MULTIPART[:PREFIX_SIZE])
            await peer.send_data(fresh, MULTIPART[PREFIX_SIZE:], end_stream=True)
            response = peer.responses[fresh]
            await peer.until(lambda: response.ended or response.reset)
            if response.reset:
                raise RuntimeError(
                    f"Fresh upload reset with HTTP/2 error {response.reset_code} (0x{response.reset_code:x})"
                )
            if response.status != b"200" or response.body != MULTIPART:
                raise RuntimeError(f"Fresh upload status={response.status!r}, bytes={len(response.body)}")
        await marker("FRESH-HEAD")
        application.stdin.close()
        await marker("CLOSED-AND-REOPENED")
        status = await asyncio.wait_for(application.wait(), 5)
        if status != 0:
            raise RuntimeError(f"Installed consumer exited {status}")
        print(f"PASS HTTP/2 reset and fresh-connection upload ({options.mode}, {options.backend})")
    finally:
        if application.returncode is None:
            application.kill()
            await application.wait()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ("consumer", "certificate", "key", "ca"):
        parser.add_argument(name, type=Path)
    parser.add_argument("--mode", choices=("poll", "timed", "service"), required=True)
    parser.add_argument("--backend", choices=("default", "uring"), default="default")
    asyncio.run(exercise(parser.parse_args()))


if __name__ == "__main__":
    main()
