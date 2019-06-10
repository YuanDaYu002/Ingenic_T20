.PHONY: all clean

#SUBDIRS=libencoder libamazon libstream libfmp4Encode  main 
SUBDIRS=libencoder  main 
	
all:
	$(foreach D, $(SUBDIRS), make -C $(D) all;)
	
clean:
	$(foreach D, $(SUBDIRS), make -C $(D) clean;)
	
