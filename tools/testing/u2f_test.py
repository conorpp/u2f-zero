from u2flib_host import u2f, exc
import sys
import requests
import json

facet = 'http://localhost:8081'
if 1:
    try:
        registrationRequest = json.loads(requests.get("http://localhost:8081/enroll").text)

        print registrationRequest

        registrationRequest = registrationRequest['registerRequests'][0]


# Enumerate available devices
        devices = u2f.list_devices()

        for device in devices:
            # The with block ensures that the device is opened and closed.
            with device as dev:
                # Register the device with some service
                print 'Reg: press button . . .'
                sys.stdout.flush()
                registrationResponse = u2f.register(device, registrationRequest, facet)
                print registrationResponse
                registrationResponse['version'] = 'U2F_V2'
                bindres = (requests.post("http://localhost:8081/bind", data={'data':json.dumps(registrationResponse)}).text)
                
                if bindres == 'true':
                    print 'Success reg'
                else:
                    print 'Fail reg'
                sys.stdout.flush()

                sign = json.loads(requests.get("http://localhost:8081/sign").text)
                key = sign['registeredKeys'][0]
                for i in key:
                    sign[i] = key[i]
                print 'Auth: press button . . . '
                sys.stdout.flush()
                auth = u2f.authenticate(device, sign, facet)
                auth['signatureData'] = auth['signatureData'].replace('1','2')
                print auth
                authres = (requests.post("http://localhost:8081/verify", data={'data':json.dumps(auth)}).text)
                try:
                    authres = json.loads(authres)
                    assert(authres['counter'] > 0)
                    print 'Success auth'
                except:
                    print 'Fail auth'

    except:
        print 'skip'

