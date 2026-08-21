# Withdrawn Candidate: Camera Demo v1.1.1

Date: 2026-08-20

No camera `v1.1.1` binary artifacts were published. The public
`camera-demo-v1.1.1` source tag is an immutable prerelease audit checkpoint,
not a released package, and this binary lane never advanced its current pointer
from `1.1.0` to `1.1.1`.

The candidate was withdrawn because its audit incorrectly claimed an
AppleClang ASan/UBSan pass even though the ASan runtime spun before test output
and was terminated. Its stager also left a temporary raw `bundles/` directory
outside the checksum set. The public prerelease erratum
[`Withdrawn candidate: camera demo v1.1.1`](https://github.com/phuong-tran/coakka-samples/releases/tag/camera-demo-v1.1.1)
records the withdrawal and has no attached artifacts.

Replacement `v1.1.2` uses a new source commit and tag, corrected sanitizer
evidence, static Windows libuv linkage, and an archive workspace outside the
promoted release directory.
