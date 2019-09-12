# RGIS installation with additional command line tools
FROM bmfekete/compute     AS rgis-compute
RUN git clone https://github.com/bmfekete/RGIS /tmp/RGIS && /tmp/RGIS/install.sh /usr/local/share && rm -rf /tmp/RGIS
ENV PATH="${PATH}:/usr/local/share/ghaas/bin:/usr/local/share/ghaas/f"

# RGIS installation with command line and desktop tools
FROM bmfekete/desktop     AS rgis-desktop
RUN git clone https://github.com/bmfekete/RGIS /tmp/RGIS && /tmp/RGIS/install.sh /usr/local/share && rm -rf /tmp/RGIS
ENV PATH="${PATH}:/usr/local/share/ghaas/bin:/usr/local/share/ghaas/f"

# RGIS installation with command line Google Cloud tools
FROM bmfekete/compute-gcp AS rgis-compute-gcp
RUN git clone https://github.com/bmfekete/RGIS /tmp/RGIS && /tmp/RGIS/install.sh /usr/local/share && rm -rf /tmp/RGIS
ENV PATH="${PATH}:/usr/local/share/ghaas/bin:/usr/local/share/ghaas/f"

# RGIS installtion with command line, desktop and Google Cloud tools
FROM bmfekete/desktop-gcp AS rgis-desktop-gcp
RUN git clone https://github.com/bmfekete/RGIS /tmp/RGIS && /tmp/RGIS/install.sh /usr/local/share && rm -rf /tmp/RGIS
ENV PATH="${PATH}:/usr/local/share/ghaas/bin:/usr/local/share/ghaas/f"
