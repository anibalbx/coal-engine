if tick > 360 then
  tick = 0
end


SetRotationTRS(light_man, {0.0,tick,0.0})

for i=1, 4 do
  SetRotationTRS(sphere_ent[i], {0.0,tick,0.0})
end


tick = tick + 1
