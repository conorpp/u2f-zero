

keyname=key.pem
certname=cert.pem
curve=prime256v1

ca:
	# generate EC private key
	openssl ecparam -genkey -name $(curve) -out $(keyname)
	# generate a "signing request"
	openssl req -new -key $(keyname) -out $(keyname).csr
	# self sign the request
	openssl x509 -req -in $(keyname).csr -signkey $(keyname) -out $(certname)

