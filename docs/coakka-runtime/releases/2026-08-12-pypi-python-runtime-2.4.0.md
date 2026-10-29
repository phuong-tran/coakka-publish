# PyPI Python Runtime 2.4.0

`coakka-v2-connector==2.4.0` is published on PyPI over native runtime
generation `2.4.0+c2f53117`.

## Release Identity

| Field | Value |
| --- | --- |
| Package | `coakka-v2-connector` |
| Registry version | `2.4.0` |
| Wheel | `coakka_v2_connector-2.4.0-py3-none-any.whl` |
| Native source | `c2f53117f991f67f809a0bf46bac2ce26091eb78` |
| Connector source | `0afb5e9a7c19d002ec4fd1cd2d1044e86b096d5e` |
| Payload staging source | `ef40ada4df41ee3fc7c9d21c2f4c4cad9c95b582` |
| Artifact generation | `2.4.0+c2f53117-0afb5e9` |
| SHA-256 | `645b83383dfdb5017b665001a364b47e2d45b5786cfeab66f65dc25df02aa370` |

## Registry Verification

PyPI JSON reports version `2.4.0`, the exact wheel filename and SHA-256 above,
and `yanked=false`. A fresh download from `files.pythonhosted.org` compares
byte-for-byte with the staged candidate. A disposable virtual environment
installs the registry wheel from `https://pypi.org/simple`, loads native
runtime `2.4.0+c2f53117`, and completes request/reply.

The published package contains Linux ARM64/x86-64, macOS ARM64, and Windows
ARM64/x86-64 native libraries. Package structure, binary format, architecture,
digest, metadata, File Lane, and Stream Lane gates passed before upload.

## Sample Evidence

The public Python basic, matched-deadletter, route hot-reload, and local desktop
CRUD samples install the exact registry coordinate in disposable environments.
Those execution results are macOS ARM64 evidence. Packaged Windows and Linux
payload checks do not by themselves claim matching-host execution.

## Install

```sh
python -m pip install coakka-v2-connector==2.4.0
```

Run the public sample with:

```sh
git clone https://github.com/phuong-tran/coakka-samples.git
cd coakka-samples
bash run.sh runtime python basic
```
