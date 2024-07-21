   10 REM lara.bas
   20 REM Sample app to illustrate Pingo 3D on Agon
   30 REM
   50 lara_vertices%=127
   60 lara_indexes%=42*3
 6130 REM
 6140 REM -- CODE --
 6150 PRINT "Reading vertices"
 6160 total_coords%=lara_vertices%*3
 6170 max_abs=-99999
 6180 DIM vertices(total_coords%)
 6190 FOR i%=0 TO total_coords%-1
 6200   READ coord
 6210   vertices(i%)=coord
 6220   a=ABS(coord)
 6230   IF a>max_abs THEN max_abs=a
 6240 NEXT i%
 6250 factor=32767.0/max_abs
 6260 PRINT "Max absolute value = ";max_abs
 6270 PRINT "Factor = ";factor
 6280 sid%=100: mid%=1: oid%=1: bmid1%=101: bmid2%=102
 6290 PRINT "Creating control structure"
 6300 scene_width%=96: scene_height%=96
 6310 VDU 23,0, &A0, sid%; &49, 0, scene_width%; scene_height%; : REM Create Control Structure
 6320 f=32767.0/256.0
 6330 distx=0*f: disty=2*f: distz=-20*f
 6340 VDU 23,0, &A0, sid%; &49, 25, distx; disty; distz; : REM Set Camera XYZ Translation Distances
 6350 pi2=PI*2.0: f=32767.0/pi2
 6360 anglex=-0.4*f
 6370 VDU 23,0, &A0, sid%; &49, 18, anglex; : REM Set Camera X Rotation Angle
 6380 PRINT "Sending vertices using factor ";factor
 6390 VDU 23,0, &A0, sid%; &49, 1, mid%; lara_vertices%; : REM Define Mesh Vertices
 6400 FOR i%=0 TO total_coords%-1
 6410   val%=vertices(i%)*factor
 6420   VDU val%;
 6430   T%=TIME
 6440   IF TIME-T%<1 GOTO 6440
 6450 NEXT i%
 6460 PRINT "Reading and sending vertex indexes"
 6470 VDU 23,0, &A0, sid%; &49, 2, mid%; lara_indexes%; : REM Set Mesh Vertex Indexes
 6480 FOR i%=0 TO lara_indexes%-1
 6490   READ val%
 6500   VDU val%;
 6510   T%=TIME
 6520   IF TIME-T%<1 GOTO 6520
 6530 NEXT i%
 6540 PRINT "Sending texture coordinate indexes"
 6550 VDU 23,0, &A0, sid%; &49, 3, mid%; 1; 32768; 32768; : REM Define Mesh Texture Coordinates
 6560 VDU 23,0, &A0, sid%; &49, 4, mid%; lara_indexes%; : REM Set Texture Coordinate Indexes
 6570 FOR i%=0 TO lara_indexes%-1
 6580   READ val%
 6585   VDU val%;
 6587   T%=TIME
 6588   IF TIME-T%<1 GOTO 6588
 6590 NEXT i%
 6600 PRINT "Creating texture bitmap"
 6610 VDU 23, 27, 0, bmid1%: REM Create a bitmap for a texture
 6620 PRINT "Setting texture pixel"
 6630 VDU 23, 27, 1, 1; 1; &55, &AA, &FF, &C0 : REM Set a pixel in the bitmap
 6640 PRINT "Create 3D object"
 6650 VDU 23,0, &A0, sid%; &49, 5, oid%; mid%; bmid1%+64000; : REM Create Object
 6660 PRINT "Scale object"
 6670 scale=6.0*256.0
 6680 VDU 23, 0, &A0, sid%; &49, 9, oid%; scale; scale; scale; : REM Set Object XYZ Scale Factors
 6690 PRINT "Create target bitmap"
 6700 VDU 23, 27, 0, bmid2% : REM Select output bitmap
 6710 VDU 23, 27, 2, scene_width%; scene_height%; &0000; &00C0; : REM Create solid color bitmap
 6720 PRINT "Render 3D object"
 6730 VDU 23, 0, &C3: REM Flip buffer
 6740 rotatex=0.0: rotatey=0.0: rotatez=0.0
 6750 incx=PI/16.0: incy=PI/32.0: incz=PI/64.0
 6760 factor=32767.0/pi2
 6770 VDU 22, 136: REM 320x240x64
 6780 VDU 23, 0, &C0, 0: REM Normal coordinates
 6790 GCOL 0,136: CLG
 6800 VDU 23, 0, &A0, sid%; &49, 38, bmid2%+64000; : REM Render To Bitmap
 6810 VDU 23, 27, 3, 50; 50; : REM Display output bitmap
 6820 VDU 23, 0, &C3: REM Flip buffer
 6830 *FX 19
 6840 rotatex=rotatex+incx: IF rotatex>=pi2 THEN rotatex=rotatex-pi2
 6850 rotatey=rotatey+incy: IF rotatey>=pi2 THEN rotatey=rotatey-pi2
 6860 rotatez=rotatez+incz: IF rotatez>=pi2 THEN rotatez=rotatez-pi2
 6870 rx=rotatex*factor: ry=rotatey*factor: rz=rotatez*factor
 6880 VDU 23, 0, &A0, sid%; &49, 13, oid%; rx; ry; rz; : REM Set Object XYZ Rotation Angles
 6890 GOTO 6790
20010 REM Vertex coordinates
20011 DATA 0.0 0.0 0.0
20012 DATA 0.360000 -2.810000 -0.200000
20013 DATA 0.360000 -3.150000 -0.180001
20014 DATA 0.130000 -3.150000 0.039999
20015 DATA 0.360000 -2.810000 -0.200000
20016 DATA 0.130000 -3.150000 0.039999
20017 DATA 0.040000 -2.810000 0.060000
20018 DATA 0.590000 -3.150000 0.039999
20019 DATA 0.360000 -3.150000 -0.180001
20020 DATA 0.360000 -2.810000 -0.200000
20021 DATA 0.590000 -3.150000 0.039999
20022 DATA 0.360000 -2.810000 -0.200000
20023 DATA 0.670000 -2.810000 0.050000
20024 DATA 0.510000 -3.140000 0.279999
20025 DATA 0.590000 -3.150000 0.039999
20026 DATA 0.670000 -2.810000 0.050000
20027 DATA 0.510000 -3.140000 0.279999
20028 DATA 0.670000 -2.810000 0.050000
20029 DATA 0.550000 -2.690000 0.380000
20030 DATA 0.160000 -2.690000 0.380000
20031 DATA 0.220000 -3.140000 0.279999
20032 DATA 0.510000 -3.140000 0.279999
20033 DATA 0.160000 -2.690000 0.380000
20034 DATA 0.510000 -3.140000 0.279999
20035 DATA 0.550000 -2.690000 0.380000
20036 DATA 0.040000 -2.810000 0.060000
20037 DATA 0.130000 -3.150000 0.039999
20038 DATA 0.220000 -3.140000 0.279999
20039 DATA 0.040000 -2.810000 0.060000
20040 DATA 0.220000 -3.140000 0.279999
20041 DATA 0.160000 -2.690000 0.380000
20042 DATA 0.430000 -2.050000 0.110000
20043 DATA 0.490000 -2.050000 -0.060000
20044 DATA 0.170000 -2.050000 -0.060000
20045 DATA 0.430000 -2.050000 0.110000
20046 DATA 0.170000 -2.050000 -0.060000
20047 DATA 0.230000 -2.050000 0.120000
20048 DATA 0.490000 -2.050000 -0.060000
20049 DATA 0.430000 -2.050000 0.110000
20050 DATA 0.550000 -2.690000 0.380000
20051 DATA 0.490000 -2.050000 -0.060000
20052 DATA 0.550000 -2.690000 0.380000
20053 DATA 0.670000 -2.810000 0.050000
20054 DATA 0.430000 -2.050000 0.110000
20055 DATA 0.230000 -2.050000 0.120000
20056 DATA 0.160000 -2.690000 0.380000
20057 DATA 0.430000 -2.050000 0.110000
20058 DATA 0.160000 -2.690000 0.380000
20059 DATA 0.550000 -2.690000 0.380000
20060 DATA 0.170000 -2.050000 -0.060000
20061 DATA 0.040000 -2.810000 0.060000
20062 DATA 0.160000 -2.690000 0.380000
20063 DATA 0.170000 -2.050000 -0.060000
20064 DATA 0.160000 -2.690000 0.380000
20065 DATA 0.230000 -2.050000 0.120000
20066 DATA 0.180000 -3.940000 -0.060001
20067 DATA 0.130000 -3.150000 0.039999
20068 DATA 0.360000 -3.150000 -0.180001
20069 DATA 0.180000 -3.940000 -0.060001
20070 DATA 0.360000 -3.150000 -0.180001
20071 DATA 0.330000 -3.940000 -0.190001
20072 DATA 0.330000 -3.940000 -0.190001
20073 DATA 0.360000 -3.150000 -0.180001
20074 DATA 0.590000 -3.150000 0.039999
20075 DATA 0.330000 -3.940000 -0.190001
20076 DATA 0.590000 -3.150000 0.039999
20077 DATA 0.490000 -3.940000 -0.060001
20078 DATA 0.490000 -3.940000 -0.060001
20079 DATA 0.590000 -3.150000 0.039999
20080 DATA 0.510000 -3.140000 0.279999
20081 DATA 0.490000 -3.940000 -0.060001
20082 DATA 0.510000 -3.140000 0.279999
20083 DATA 0.430000 -3.940000 0.159999
20084 DATA 0.430000 -3.940000 0.159999
20085 DATA 0.510000 -3.140000 0.279999
20086 DATA 0.220000 -3.140000 0.279999
20087 DATA 0.430000 -3.940000 0.159999
20088 DATA 0.220000 -3.140000 0.279999
20089 DATA 0.230000 -3.940000 0.159999
20090 DATA 0.240000 -4.050000 -0.080001
20091 DATA 0.450000 -4.050000 -0.090001
20092 DATA 0.350000 -4.050000 0.099999
20093 DATA 0.490000 -2.050000 -0.060000
20094 DATA 0.330000 -2.050000 -0.230000
20095 DATA 0.170000 -2.050000 -0.060000
20096 DATA 0.360000 -2.810000 -0.200000
20097 DATA 0.170000 -2.050000 -0.060000
20098 DATA 0.330000 -2.050000 -0.230000
20099 DATA 0.360000 -2.810000 -0.200000
20100 DATA 0.040000 -2.810000 0.060000
20101 DATA 0.170000 -2.050000 -0.060000
20102 DATA 0.490000 -2.050000 -0.060000
20103 DATA 0.670000 -2.810000 0.050000
20104 DATA 0.360000 -2.810000 -0.200000
20105 DATA 0.360000 -2.810000 -0.200000
20106 DATA 0.330000 -2.050000 -0.230000
20107 DATA 0.490000 -2.050000 -0.060000
20108 DATA 0.240000 -4.050000 -0.080001
20109 DATA 0.180000 -3.940000 -0.060001
20110 DATA 0.330000 -3.940000 -0.190001
20111 DATA 0.450000 -4.050000 -0.090001
20112 DATA 0.330000 -3.940000 -0.190001
20113 DATA 0.490000 -3.940000 -0.060001
20114 DATA 0.450000 -4.050000 -0.090001
20115 DATA 0.240000 -4.050000 -0.080001
20116 DATA 0.330000 -3.940000 -0.190001
20117 DATA 0.430000 -3.940000 0.159999
20118 DATA 0.350000 -4.050000 0.099999
20119 DATA 0.450000 -4.050000 -0.090001
20120 DATA 0.450000 -4.050000 -0.090001
20121 DATA 0.490000 -3.940000 -0.060001
20122 DATA 0.430000 -3.940000 0.159999
20123 DATA 0.350000 -4.050000 0.099999
20124 DATA 0.430000 -3.940000 0.159999
20125 DATA 0.230000 -3.940000 0.159999
20126 DATA 0.240000 -4.050000 -0.080001
20127 DATA 0.230000 -3.940000 0.159999
20128 DATA 0.180000 -3.940000 -0.060001
20129 DATA 0.240000 -4.050000 -0.080001
20130 DATA 0.350000 -4.050000 0.099999
20131 DATA 0.230000 -3.940000 0.159999
20132 DATA 0.220000 -3.140000 0.279999
20133 DATA 0.180000 -3.940000 -0.060001
20134 DATA 0.230000 -3.940000 0.159999
20135 DATA 0.180000 -3.940000 -0.060001
20136 DATA 0.220000 -3.140000 0.279999
20137 DATA 0.130000 -3.150000 0.039999
20138 REM Texture coordinates
20139 DATA 0.0 0.0
20140 DATA 0.968627 0.759563
20141 DATA 0.968627 0.759563
20142 DATA 0.968627 0.759563
20143 DATA 0.968627 0.759563
20144 DATA 0.968627 0.759563
20145 DATA 0.968627 0.759563
20146 DATA 0.968627 0.759563
20147 DATA 0.968627 0.759563
20148 DATA 0.968627 0.759563
20149 DATA 0.968627 0.759563
20150 DATA 0.968627 0.759563
20151 DATA 0.968627 0.759563
20152 DATA 0.968627 0.759563
20153 DATA 0.968627 0.759563
20154 DATA 0.968627 0.759563
20155 DATA 0.968627 0.759563
20156 DATA 0.968627 0.759563
20157 DATA 0.968627 0.759563
20158 DATA 0.968627 0.759563
20159 DATA 0.968627 0.759563
20160 DATA 0.968627 0.759563
20161 DATA 0.968627 0.759563
20162 DATA 0.968627 0.759563
20163 DATA 0.968627 0.759563
20164 DATA 0.968627 0.759563
20165 DATA 0.968627 0.759563
20166 DATA 0.968627 0.759563
20167 DATA 0.968627 0.759563
20168 DATA 0.968627 0.759563
20169 DATA 0.968627 0.759563
20170 DATA 0.968627 0.759563
20171 DATA 0.968627 0.759563
20172 DATA 0.968627 0.759563
20173 DATA 0.968627 0.759563
20174 DATA 0.968627 0.759563
20175 DATA 0.968627 0.759563
20176 DATA 0.968627 0.759563
20177 DATA 0.968627 0.759563
20178 DATA 0.968627 0.759563
20179 DATA 0.968627 0.759563
20180 DATA 0.968627 0.759563
20181 DATA 0.968627 0.759563
20182 DATA 0.968627 0.759563
20183 DATA 0.968627 0.759563
20184 DATA 0.968627 0.759563
20185 DATA 0.968627 0.759563
20186 DATA 0.968627 0.759563
20187 DATA 0.968627 0.759563
20188 DATA 0.968627 0.759563
20189 DATA 0.968627 0.759563
20190 DATA 0.968627 0.759563
20191 DATA 0.968627 0.759563
20192 DATA 0.968627 0.759563
20193 DATA 0.968627 0.759563
20194 DATA 0.054902 0.442623
20195 DATA 0.054902 0.644809
20196 DATA 0.003922 0.644809
20197 DATA 0.054902 0.442623
20198 DATA 0.003922 0.644809
20199 DATA 0.003922 0.442623
20200 DATA 0.003922 0.442623
20201 DATA 0.003922 0.644809
20202 DATA 0.054902 0.644809
20203 DATA 0.003922 0.442623
20204 DATA 0.054902 0.644809
20205 DATA 0.054902 0.442623
20206 DATA 0.996078 0.792350
20207 DATA 0.996078 0.994536
20208 DATA 0.945098 0.994536
20209 DATA 0.996078 0.792350
20210 DATA 0.945098 0.994536
20211 DATA 0.945098 0.792350
20212 DATA 0.996078 0.792350
20213 DATA 0.996078 0.994536
20214 DATA 0.945098 0.994536
20215 DATA 0.996078 0.792350
20216 DATA 0.945098 0.994536
20217 DATA 0.945098 0.792350
20218 DATA 0.956863 0.759563
20219 DATA 0.956863 0.759563
20220 DATA 0.956863 0.759563
20221 DATA 0.968627 0.759563
20222 DATA 0.968627 0.759563
20223 DATA 0.968627 0.759563
20224 DATA 0.968627 0.759563
20225 DATA 0.968627 0.759563
20226 DATA 0.968627 0.759563
20227 DATA 0.968627 0.759563
20228 DATA 0.968627 0.759563
20229 DATA 0.968627 0.759563
20230 DATA 0.968627 0.759563
20231 DATA 0.968627 0.759563
20232 DATA 0.968627 0.759563
20233 DATA 0.968627 0.759563
20234 DATA 0.968627 0.759563
20235 DATA 0.968627 0.759563
20236 DATA 0.956863 0.759563
20237 DATA 0.956863 0.759563
20238 DATA 0.956863 0.759563
20239 DATA 0.956863 0.759563
20240 DATA 0.956863 0.759563
20241 DATA 0.956863 0.759563
20242 DATA 0.956863 0.759563
20243 DATA 0.956863 0.759563
20244 DATA 0.956863 0.759563
20245 DATA 0.956863 0.759563
20246 DATA 0.956863 0.759563
20247 DATA 0.956863 0.759563
20248 DATA 0.956863 0.759563
20249 DATA 0.956863 0.759563
20250 DATA 0.956863 0.759563
20251 DATA 0.956863 0.759563
20252 DATA 0.956863 0.759563
20253 DATA 0.956863 0.759563
20254 DATA 0.956863 0.759563
20255 DATA 0.956863 0.759563
20256 DATA 0.956863 0.759563
20257 DATA 0.956863 0.759563
20258 DATA 0.956863 0.759563
20259 DATA 0.956863 0.759563
20260 DATA 0.996078 0.994536
20261 DATA 0.945098 0.792350
20262 DATA 0.996078 0.792350
20263 DATA 0.945098 0.792350
20264 DATA 0.996078 0.994536
20265 DATA 0.945098 0.994536
20266 REM Vertex indexes
20267 DATA 1 2 3
20268 DATA 4 5 6
20269 DATA 7 8 9
20270 DATA 10 11 12
20271 DATA 13 14 15
20272 DATA 16 17 18
20273 DATA 19 20 21
20274 DATA 22 23 24
20275 DATA 25 26 27
20276 DATA 28 29 30
20277 DATA 31 32 33
20278 DATA 34 35 36
20279 DATA 37 38 39
20280 DATA 40 41 42
20281 DATA 43 44 45
20282 DATA 46 47 48
20283 DATA 49 50 51
20284 DATA 52 53 54
20285 DATA 55 56 57
20286 DATA 58 59 60
20287 DATA 61 62 63
20288 DATA 64 65 66
20289 DATA 67 68 69
20290 DATA 70 71 72
20291 DATA 73 74 75
20292 DATA 76 77 78
20293 DATA 79 80 81
20294 DATA 82 83 84
20295 DATA 85 86 87
20296 DATA 88 89 90
20297 DATA 91 92 93
20298 DATA 94 95 96
20299 DATA 97 98 99
20300 DATA 100 101 102
20301 DATA 103 104 105
20302 DATA 106 107 108
20303 DATA 109 110 111
20304 DATA 112 113 114
20305 DATA 115 116 117
20306 DATA 118 119 120
20307 DATA 121 122 123
20308 DATA 124 125 126
20309 REM Texture indexes
20310 DATA 1 2 3
20311 DATA 4 5 6
20312 DATA 7 8 9
20313 DATA 10 11 12
20314 DATA 13 14 15
20315 DATA 16 17 18
20316 DATA 19 20 21
20317 DATA 22 23 24
20318 DATA 25 26 27
20319 DATA 28 29 30
20320 DATA 31 32 33
20321 DATA 34 35 36
20322 DATA 37 38 39
20323 DATA 40 41 42
20324 DATA 43 44 45
20325 DATA 46 47 48
20326 DATA 49 50 51
20327 DATA 52 53 54
20328 DATA 55 56 57
20329 DATA 58 59 60
20330 DATA 61 62 63
20331 DATA 64 65 66
20332 DATA 67 68 69
20333 DATA 70 71 72
20334 DATA 73 74 75
20335 DATA 76 77 78
20336 DATA 79 80 81
20337 DATA 82 83 84
20338 DATA 85 86 87
20339 DATA 88 89 90
20340 DATA 91 92 93
20341 DATA 94 95 96
20342 DATA 97 98 99
20343 DATA 100 101 102
20344 DATA 103 104 105
20345 DATA 106 107 108
20346 DATA 109 110 111
20347 DATA 112 113 114
20348 DATA 115 116 117
20349 DATA 118 119 120
20350 DATA 121 122 123
20351 DATA 124 125 126