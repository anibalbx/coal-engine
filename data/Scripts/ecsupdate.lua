--timesec = os.clock()


--SetRotationTRS(myentity, { 0.0 , timesec * 100, 0.0})


if tick > 360 then
  tick = 0
end

for i = 0, 1, 1 do
    for j = 0, 1, 1 do
        SetRotationTRS(sphere_array[i][j], {0.0,tick,0.0})
    end
end

tick = tick + 1
--myposition = GetPosition(myentity)
--print ("position x "..myposition[1].." position y "..myposition[2].." position z "..myposition[3])
--scale = GetScale(myentity)
--print ("s x "..scale[1].." s y "..scale[2].." s z "..scale[3])
--rotation = GetRotation(myentity)
--print ("s x "..rotation[1].." s y "..rotation[2].." s z "..rotation[3])

--SetRotationTRS(otherentity, { 0.0 , timesec * 100, 0.0})
--if (timesec>2 and otherentity ~= 0) then
--otherentity = DeleteEntity(otherentity)

--end
