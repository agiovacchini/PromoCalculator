def power(num, pow)
   num ** pow
end

def getDistanceBetweenCoords( lat1, long1, lat2, long2 )
	dtor = Math::PI/180
	r = 6371*1000
	rlat1 = lat1.to_f * dtor.to_f
	rlong1 = long1.to_f * dtor.to_f
	rlat2 = lat2 * dtor 
	rlong2 = long2 * dtor 
	dlon = rlong1 - rlong2
	dlat = rlat1 - rlat2
	#puts "Dlon: " << dlon.to_s
	#puts "Dlat: " << dlat.to_s
	a = power(Math::sin(dlat/2), 2) + Math::cos(rlat1) * Math::cos(rlat2) * power(Math::sin(dlon/2), 2)
	c = 2 * Math::atan2(Math::sqrt(a), Math::sqrt(1-a))
	distance = r * c
end
