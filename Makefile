#!/bin/sh

# remove all generated files
clean:
	find . -name "*.bool" -delete
	find . -name "*.cex" -delete
	find . -name "*.quabs" -delete
	find . -name "*.qcir" -delete
	find . -name "*.cex" -delete
