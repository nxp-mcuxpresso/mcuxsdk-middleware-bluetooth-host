

import time

import ranging

# python -m cProfile demo.py

print("Settings ---------------")
w = ranging.Worker()
w.showSettings()

print("---------------------------------")

hest_in = [complex(181.665131,-130.046967), complex(87.670372,-253.580536), complex(-39.232094,-299.601654), complex(-191.006592,-294.843231), complex(-312.997192,-212.695206), complex(-384.380249,-84.718193), complex(-406.576111,23.940166), complex(-377.659393,122.141808), complex(-303.178497,211.426605), complex(-195.501114,229.359299), complex(-155.591675,190.556473), complex(-162.275742,148.389404), complex(-189.088913,133.905258), complex(-198.242477,176.675049), complex(-186.917450,226.720398), complex(-173.799957,272.819397), complex(-149.537354,301.546722), complex(-100.971474,350.772339), complex(-62.212410,369.009979), complex(21.351336,365.223053), complex(94.563545,314.344177), complex(118.141769,238.378860), complex(101.002831,197.662781), complex(62.266304,174.034744), complex(44.903999,180.139862), complex(15.572072,200.647675), complex(46.320892,235.379745), complex(64.162201,243.749435), complex(76.456970,259.943604), complex(82.269005,306.822723), complex(127.985550,316.852173), complex(201.893341,349.283447), complex(258.369232,372.366302), complex(349.940216,348.409515), complex(464.659668,276.773529), complex(510.949341,225.491043), complex(543.602539,97.446198), complex(540.481323,-26.722477), complex(524.589661,-58.739243), complex(529.739197,-169.509827), complex(521.519043,-270.357330), complex(436.571686,-372.527588), complex(332.213501,-442.819153), complex(258.876770,-463.513947), complex(128.591232,-503.140839), complex(23.741333,-513.724304), complex(-60.408722,-467.813232), complex(-162.863831,-353.801697), complex(-165.545761,-252.682404), complex(-123.791527,-212.146988), complex(-100.179588,-195.803360), complex(-92.690727,-188.967117), complex(-73.349144,-168.250092), complex(-37.776009,-231.654968), complex(-64.947716,-288.609070), complex(-106.654213,-353.774109), complex(-234.396240,-363.930756), complex(-340.427521,-325.104431), complex(-417.843353,-284.561188), complex(-500.770081,-197.627625), complex(-544.093994,-122.760956), complex(-575.863831,79.932091), complex(-542.834290,185.426758), complex(-476.700134,304.067474), complex(-360.424347,385.789703), complex(-245.837982,407.357727), complex(-155.638489,406.920563), complex(-109.766991,376.734375), complex(-85.957397,389.419647), complex(-54.516350,411.289459), complex(20.031519,414.771332), complex(85.313957,392.128113), complex(144.543152,346.892944), complex(170.819229,319.682983), complex(205.310852,259.311676), complex(210.470795,246.924988), complex(227.974228,206.468521), complex(225.059982,148.627045), complex(191.259399,144.855621), complex(187.722412,125.501793)]
#EVD  Distance Hest: 6.578085 meter     run: 656564 us
#FAST Distance Hest: 6.568936 meter     run: 1423246 us

print(len(hest_in))
print(hest_in)

# Create a Hest
Hest = ranging.CmplxMatrixType(1, len(hest_in))

# copy the data into the sample array's
for x in range(len(Hest.data)):
    Hest.data[x] = hest_in[x]

# md = ranging.MusicData(1, len(hest_in), 1.0e+6, 40)
# 
# md.hest = Hest
# 
# dist_est = w.calculateDistance(md, 0)
# print("Distance:  " , dist_est)


print("---------------------------------")

# Create a measurement instance
md = ranging.MusicData(1, len(hest_in), 1.0e+6, 40)

# copy the data into the sample array's
md.hest = Hest

dist_est = w.calculateDistance(md,0)
print("Distance EVD:  " , dist_est)

for x in range(len(md.EigenValues)):
    print(md.EigenValues[x])

# for x in range(len(md.EigenVects.data)):
#    print md.EigenVects.data[x]

#print(md.EigenVects.rows())
#print(md.EigenVects.cols())

#print(len(md.EigenValues))

print("---------------------------------")

# Reset measurement instance to prepare for new distance calculation
md.reset()

# calculate with FAST evd enabled
dist_est = w.calculateDistance(md,1)
print("Distance FAST:  " , dist_est)

print("End")
