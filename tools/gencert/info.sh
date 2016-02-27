#!/bin/bash

openssl x509 -in $1 -inform der -text
