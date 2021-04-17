FROM wiiuenv/devkitppc:20210414

COPY --from=wiiuenv/wiiumodulesystem:20210414 /artifacts $DEVKITPRO

WORKDIR project