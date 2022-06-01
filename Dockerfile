FROM ubuntu:20.04 AS base
RUN apt-get update && DEBIAN_FRONTEND=noninteractive \
    apt-get install -y tzdata gnupg gnupg-utils lsb-release wget ca-certificates apt-transport-https curl bc
ENTRYPOINT ["/bin/bash"]

# RGIS container
FROM base AS rgis
RUN apt-get update && DEBIAN_FRONTEND=noninteractive \
    apt-get install -y --no-install-recommends libudunits2-0 libudunits2-data libexpat1 libmotif-common libxss1 \
            git cmake clang make libnetcdf-dev libudunits2-dev libgdal-dev libexpat1-dev libxext-dev libmotif-dev
RUN git clone https://github.com/bmfekete/RGIS /tmp/RGIS && /tmp/RGIS/install.sh /usr/local/share && rm -rf /tmp/RGIS
RUN apt-get purge -y git cmake clang make libnetcdf-dev libudunits2-dev libgdal-dev libexpat1-dev libxext-dev libmotif-dev
ENV PATH="${PATH}:/usr/local/share/ghaas/bin:/usr/local/share/ghaas/f"