FROM bmfekete/desktop

RUN git clone https://github.com/bmfekete/RGIS /tmp/RGIS && /tmp/RGIS/install.sh /usr/local/share && rm -rf /tmp/RGIS

ENV PATH="${PATH}:/usr/local/share/ghaas/bin:/usr/local/share/ghaas/f"

ENTRYPOINT ["/bin/bash"]
