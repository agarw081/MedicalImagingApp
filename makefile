all:
	cd MedImage; $(MAKE)
	cd PhotoShop; $(MAKE)
	cd photolib; $(MAKE)
	cd tests; $(MAKE)

clean:
	cd photolib; $(MAKE) clean
	cd MedImage; $(MAKE) clean
	cd PhotoShop; $(MAKE) clean
	cd tests; $(MAKE) clean
	
