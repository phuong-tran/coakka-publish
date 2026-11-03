"""Serve the fixed release-confirmation response through the Python connector."""

import hashlib
import json
import os
from pathlib import Path
import signal
import sys
import threading
from dataclasses import asdict
import coakka_http
from coakka_http import (
    Builder,
    CpuPolicy,
    Header,
    Headers,
    Limits,
    NotificationProfile,
    Response,
)


def identify(kind, path):
    path = Path(path).resolve(strict=True)
    print(f"identity-{kind}={path}\t{hashlib.sha256(path.read_bytes()).hexdigest()}")


# Keep signal handling separate from closing the service.
stop = threading.Event()
signal.signal(signal.SIGINT, lambda *_: stop.set())
signal.signal(signal.SIGTERM, lambda *_: stop.set())
headers = Headers((Header("content-type", "application/octet-stream"),))
body = b"0123456789abcdef0123456789abcdef"


def make():
    return Response(headers=headers, body=body)


shared = make()
handler = (
    (lambda _: make()) if os.environ["RESPONSE_MODE"] == "fresh" else (lambda _: shared)
)
service = (
    Builder()
    .listen("127.0.0.1", 18430)
    .cpu(CpuPolicy.AUTO)
    .concurrency(1)
    .limits(
        Limits(
            request_notification_profile=NotificationProfile.ULTRA,
            terminal_notification_profile=NotificationProfile.XXLARGE,
        )
    )
    .get("/fixed", handler)
    .start()
)
try:
    # Record the imported Python files and the native images mapped by this process.
    identify("fixture", __file__)
    paths = {
        Path(m.__file__).resolve()
        for name, m in tuple(sys.modules.items())
        if (name == "coakka_http" or name.startswith("coakka_http."))
        and getattr(m, "__file__", None)
        and str(m.__file__).endswith(".py")
    }
    for path in sorted(paths):
        identify("module", path)
    with open("/proc/self/maps") as maps:
        images = {
            line[line.index("/") :].strip()
            for line in maps
            if "/" in line and "coakka_http" in line
        }
    for path in sorted(images):
        identify("image", path)
    print("coakka-runtime-info=" + json.dumps(asdict(service.runtime_info())))
    stop.wait()
    error = service.poll_error()
    if error is not None:
        raise RuntimeError("handler diagnostic") from error
finally:
    service.close()
print("benchmark-shutdown=pass")
