# Using the console

To start the measurement on the reflector side:
```
\> test rx
```
To start the measurement on the initiator side:
```
\> test tx
```
Both devices synchronize to each other and perform a CS test procedure using default settings.

The expected output \(on both devices\) is a JSON-like structure similar to the following:

```
items:[{hadm:{cfg:{rtyp:0,rphy:0,txpwr:0,fcs:150,ip1:145,ip2:145,tpm:20,ant:7},sts:0,stp:{nb:101,md:'00022221222212222122221222212222122221222212222122221222212222122221222212222122221222212222122221222',ch:'0001020001020303040506070708090A0B0B0C0D0E0F0F1011121313141516171718191A1B1B1C1D1E1F1F2021222323242526272728292A2B2B2C2D2E2F2F3031323333343536373738393A3B3B3C3D3E3F3F4041424343444546474748494A4B4B4C4D4E',evt:'00',se:'65'}},md0:{cfg:{n_stp:3},init:{r:'808080',c:'800080008000'},refl:{r:'5A555D',c:'7FFF7FFF7FFF'}},mciq:{cfg:{n_ap:4,n_stp:79},refl:{i:['lMjijwmvkNmoZOjCmxkglIcvkIZebvgqiMnAZGj1eFb+l1ZbjSkIljmMhDcgmEe9mQbNfTegk8azh1aWfFcvmEdobceLe8iEizfle1lWcCbYg1eWdKdRZtgeiqcrZ6hSgtdGlbeVhelSazfdg6lRjvjic3i3kT','kBgKgOj6gzkacUjpkUkJhmf4g4cEcGiKfQj0c9kChpgDh6b/kik4iJjjeDbakFhKlCdVeKgBkHdPiMb/gqfOkJf/eIdVd0iogzgxeLkEd9djhHeTdtfLcmhNgafUcuh/fKfcjfd9fyjUcvgufVi/iyiudXgwiT','XVmVmLdZlUbKhgWvb+XfkDZcltiaohXLnMdHgiYiYzakifhdZcaIjRhtmolXiSZIgtaanBYicyaoZVheYTYofaYWZhmdm9Zon9X/nZeZYhZUYam3l3YVeYY6nVY8efaNmjZZgnksmMhDfQaRlvihe3e5hglOkE','w6X7X8oYaZr/Y4xKrBwtcyoVZyX1PYvAVKpBa6wTr5m6fyX4vovEeyijTtRXiksimrkiRqsJs/i0u+UtsPoWpfqRlMRzR9uQW5srSEq8mSkUtNSaSlpUaXsMW2n+bdr9VFoMmPU8WdlVaWp+WUimnXneYZY8dl',],q:['bGmDlwgNlUegfHZ3exa1kfaMlMf5lHZSmlfnepaKZdaojTgVaWa/jThwmXlYh9ZwffcCmLaDcYcwaIiYZ7ateQaJbtl+mIaHlpZzmFdAbVb4ZymIltaTgVZvluaxfiaGl7bBeQlbldeLiBailXfZcdcdjtjthX','ezkUkdiSkfhFdcdYhUeUkNbhkceDh3cYkNh6dEebb6bdkVdZeKeukvj6lBiyjea2hhbpkobVeRc4cfgkcCcBgWbzcKjdj2cgkXbvj7e1cfc7cWjWi3cjfWc1jbcpezdIjbcjgGixjagBgOc6jGgse8e0hbi7h+','bvY4YwW4YMX6pZeCXecAXmmpYnpAjeh1bBYCoSc4jXloY2nUdCcHZ9ZffTkGZpgGZDkiiNgZZNlYcYnifpieYRhNkJkUjpbUeleRjwX4jjkwdNkFlRhMniddfChkm/cIiAg1ZikchXZ0mPdniYafaQadlRfxdI','kJvVvUvHwTsbQVfKtDjLwpRRvzRPdlYsslt6QViLUyRYwDSZhMjivNu1oYeVujYHtaRvkgXHnpRjePWZXmT5rIVzSmhuive6rBZdi7pKTWSrb2hhe9WlUTcLorWgUtfFkcX6pdfXlWpbXEdFjNpqmamGaUmMpA',],tqi:['00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','00000000000000000000000000000000000000000000000200000000000000000000000000020202000000000000000000000000000000000000000000000000000000000000000000000000000000','00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000',],},},tof:{cfg:{n_stp:19},refl:{d:'AP+AAP+EAP+NAP+YAP+hAP+qAP+vAP/BAP/DAP/RAP/YAP/kAP/oAP/yAAAAAAAIAAASAAAVAAAl',r:'625A565C6159585A6159565B6059545A5E5853',nadm:'FFFFFFFFFFFFFFFFFFF'},},info:{init:{},refl:{syn:0,syg:0,syr:0,syc:0,f:0x0000,x:0,ta:0,te:0,},},},]
CRC32:17002f48
marker:[DONE]
```

It contains all the CS results returned locally by HCI events and debug data, but does not show the final distance. In this mode, the host PC can only compute the final distance.

See [Command response](command_response.md) for how to interpret these results.

**Parent topic:**[Operating test mode](../topics/operating_testmode.md)

