all:
	$(MAKE) -C src
	$(MAKE) -C client

clean:
	rm -f MerDeGlaced MdG src/*.o
