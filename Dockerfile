FROM ghcr.io/wiiu-env/devkitppc:20220806

COPY --from=ghcr.io/wiiu-env/wiiumodulesystem:20220904 /artifacts $DEVKITPRO

WORKDIR project
