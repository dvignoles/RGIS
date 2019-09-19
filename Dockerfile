FROM ubuntu:19.04 AS base
RUN apt-get update && DEBIAN_FRONTEND=noninteractive \
    apt-get install -y tzdata gnupg gnupg-utils lsb-release wget ca-certificates apt-transport-https
ENTRYPOINT ["/bin/bash"]

# Minimum container with basic command-line applications
FROM base AS compute
RUN wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc | apt-key add - && \
    echo "deb http://apt.postgresql.org/pub/repos/apt/ $(lsb_release -cs)-pgdg main" >> /etc/apt/sources.list.d/pgdg.list && \
    apt-get update && DEBIAN_FRONTEND=noninteractive \
    apt-get install -y --no-install-recommends dialog apt-utils apt-transport-https dnsutils psmisc \
                       vim rsync screen tmux bc curl nfs-common python python-numpy python-psutil \
                       netcdf-bin nco cdo libudunits2-0 libudunits2-data gnuplot imagemagick \
	                     gdal-bin python-gdal python3-gdal postgresql-client-11 libexpat1 \
                       libmotif-common libxss1 r-base texlive-full && rm -rf /var/lib/apt/lists/*

# Container with basic command-line and desktop applications
FROM compute AS desktop
RUN echo "deb https://qgis.org/debian $(lsb_release -cs) main" > /etc/apt/sources.list.d/qgis.list && \
    apt-key adv --keyserver keyserver.ubuntu.com --recv-key 51F523511C7028C3 && \
    apt-get update && DEBIAN_FRONTEND=noninteractive \
    apt-get install -y xbase-clients xvfb xfonts-100dpi xfonts-100dpi-transcoded \
            xfonts-75dpi xfonts-75dpi-transcoded xfce4 xfce4-goodies qgis imagemagick && \
    curl -o /tmp/chrome-remote-desktop_current_amd64.deb \
         http://dl.google.com/linux/direct/chrome-remote-desktop_current_amd64.deb && \
    dpkg -i /tmp/chrome-remote-desktop_current_amd64.deb && \
    rm -rf /tmp/chrome-remote-desktop_current_amd64.deb /var/lib/apt/lists/*

# Compute container with Google Cloud Platform tools
FROM compute AS compute-gcp
RUN echo "deb http://packages.cloud.google.com/apt cloud-sdk main" > /etc/apt/sources.list.d/google.list && \
    echo "deb http://packages.cloud.google.com/apt gcsfuse-bionic main" >> /etc/apt/sources.list.d/google.list && \
    curl -o /tmp/apt-key.gpg https://packages.cloud.google.com/apt/doc/apt-key.gpg && \ 
    cat /tmp/apt-key.gpg | apt-key add - && \
    cat /tmp/apt-key.gpg | apt-key --keyring /usr/share/keyrings/cloud.google.gpg add - && \
    apt-get update && DEBIAN_FRONTEND=noninteractive \
    apt-get install -y micro-httpd google-cloud-sdk gcsfuse && apt-get -y autoremove && \
    rm -rf /tmp/apt-key.gpg /var/lib/apt/lists/*

# Desktop container with Google Cloud Platform tools
FROM desktop AS desktop-gcp
RUN echo "deb http://packages.cloud.google.com/apt cloud-sdk main" > /etc/apt/sources.list.d/google.list && \
    echo "deb http://packages.cloud.google.com/apt gcsfuse-bionic main" >> /etc/apt/sources.list.d/google.list && \
    curl -o /tmp/apt-key.gpg https://packages.cloud.google.com/apt/doc/apt-key.gpg && \ 
    cat /tmp/apt-key.gpg | apt-key add - && \
    cat /tmp/apt-key.gpg | apt-key --keyring /usr/share/keyrings/cloud.google.gpg add - && \
    apt-get update && DEBIAN_FRONTEND=noninteractive \
    apt-get install -y micro-httpd google-cloud-sdk gcsfuse && apt-get -y autoremove && \
    rm -rf /tmp/apt-key.gpg /var/lib/apt/lists/*

# Auxiliary container to build RGIS 
FROM base AS rgis
RUN apt-get update && DEBIAN_FRONTEND=noninteractive \
    apt-get install -y --no-install-recommends \
            git cmake clang curl libnetcdf-dev libudunits2-dev libgdal-dev \
            libexpat1-dev libxext-dev libmotif-dev
RUN git clone https://github.com/bmfekete/RGIS /tmp/RGIS && /tmp/RGIS/install.sh /usr/local/share && rm -rf /tmp/RGIS
ENV PATH="${PATH}:/usr/local/share/ghaas/bin:/usr/local/share/ghaas/f"

FROM compute     AS rgis-compute
  COPY --from=rgis /usr/local/share/ghaas /usr/local/share/ghaas
  ENV PATH=\"${PATH}:/usr/local/share/ghaas/bin:/usr/local/share/ghaas/f\"

FROM compute-gcp AS rgis-compute-gcp
  COPY --from=rgis /usr/local/share/ghaas /usr/local/share/ghaas
  ENV PATH=\"${PATH}:/usr/local/share/ghaas/bin:/usr/local/share/ghaas/f\"

FROM desktop     AS rgis-desktop
  COPY --from=rgis /usr/local/share/ghaas /usr/local/share/ghaas
  ENV PATH=\"${PATH}:/usr/local/share/ghaas/bin:/usr/local/share/ghaas/f\"

FROM desktop-gcp AS rgis-desktop-gcp
  COPY --from=rgis /usr/local/share/ghaas /usr/local/share/ghaas
  ENV PATH=\"${PATH}:/usr/local/share/ghaas/bin:/usr/local/share/ghaas/f\"
