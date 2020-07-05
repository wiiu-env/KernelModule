Run via [SetupPayload](https://github.com/wiiu-env/SetupPayload). Requires [wut](https://github.com/decaf-emu/wut), [wums](https://github.com/wiiu-env/WiiUModuleSystem)

## Building using the Dockerfile

It's possible to use a docker image for building. This way you don't need anything installed on your host system.

```
# Build docker image (only needed once)
docker build . -t kernelmodule-builder

# make 
docker run -it --rm -v ${PWD}:/project kernelmodule-builder make

# make clean
docker run -it --rm -v ${PWD}:/project kernelmodule-builder make clean
```