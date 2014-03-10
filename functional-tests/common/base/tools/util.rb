def getBlankOrValue( pVal )
  return pVal.nil? ? '' : pVal
end

def signedIntFromCharArray ( pCharArray )

  numFromBytes = pCharArray.pack("C*").unpack('L').join.to_i

  length = pCharArray.length * 8
  maxNum = 2**length
  midNum = 2**(length-1)

  #puts "numFromBytes: #{numFromBytes}"
  #puts "maxNum: #{maxNum}"
  if numFromBytes < midNum
    num = numFromBytes
  elsif numFromBytes > midNum
    num = numFromBytes - maxNum
  else
    num = 0
  end

  return num

end
