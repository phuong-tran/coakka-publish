#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
root="$PWD"
mkdir -p inputs sdk jvm classes
fetch() {
  local url=$1 name=${1##*/}
  if [[ ! -f inputs/$name ]]; then
    curl --fail --silent --show-error --location --retry 2 --max-time 120 \
      --output "inputs/$name.download" "$url"
    mv "inputs/$name.download" "inputs/$name"
  fi
}
sdk=coakka-http-runtime-native-2.2.0-linux-aarch64.tar.gz
fetch "https://github.com/phuong-tran/coakka-http-runtime-go/releases/download/v2.2.1/$sdk"
for engine in node bun; do
  package="coakka-http-runtime-$engine"
  fetch "https://registry.npmjs.org/$package/-/$package-2.2.1.tgz"
done
fetch https://files.pythonhosted.org/packages/f4/02/d708d7f2b65b7b4fdae1d30c69b8cb402739022c0c8a1f3788d42d425036/coakka_http_runtime_python-2.2.1-cp311-abi3-manylinux_2_28_aarch64.whl
maven=https://repo.maven.apache.org/maven2
jvm_release="$maven/io/github/phuong-tran/coakka/coakka-http-runtime-jvm/2.2.1"
fetch "$jvm_release/coakka-http-runtime-jvm-2.2.1.jar"
fetch "$jvm_release/coakka-http-runtime-jvm-2.2.1-linux-aarch64.zip"
fetch "$maven/org/jetbrains/kotlin/kotlin-stdlib/2.3.10/kotlin-stdlib-2.3.10.jar"
(cd inputs && sha256sum -c ../inputs.sha256)
tar -C sdk -xzf "inputs/$sdk"
unzip -oq inputs/coakka-http-runtime-jvm-2.2.1-linux-aarch64.zip -d jvm
for engine in node bun; do
  package="$engine/node_modules/coakka-http-runtime-$engine"
  mkdir -p "$package"
  tar -C "$package" -xzf "inputs/coakka-http-runtime-$engine-2.2.1.tgz" --strip-components=1
done
python3.13 -m venv python
python/bin/pip install --no-index --no-deps inputs/*.whl
gcc -std=c11 -D_POSIX_C_SOURCE=200809L -O3 -Wall -Wextra -Werror \
  -Isdk/include fixture.c -Lsdk/lib -Wl,-rpath,"$root/sdk/lib" \
  -lcoakka_http_host -o native-fixed
javac -cp inputs/coakka-http-runtime-jvm-2.2.1.jar:inputs/kotlin-stdlib-2.3.10.jar \
  -d classes FixedServer.java
export GOCACHE="$root/go-cache" GOMODCACHE="$root/go-mod-cache"
export CGO_ENABLED=1 CGO_CFLAGS="-I$root/sdk/include"
export CGO_LDFLAGS="-L$root/sdk/lib -Wl,-rpath,$root/sdk/lib" GOTOOLCHAIN=local
(cd go-app && "${BENCHMARK_GO:-go}" build -mod=readonly -o ../go-fixed .)
"${BENCHMARK_GO:-go}" version -m go-fixed
