test_man = CreateEntity()
NewLocalTransform(test_man)


light_man = CreateEntity()
NewWorldTransform(light_man)
NewLocalTransform(light_man)
NewLocalTransformTRS(light_man)


math.randomseed(os.time()) -- random initialize
math.random(); math.random(); math.random() -- warming up

plane_mat = CreatePBRMaterial();

plane_albedo = CreateTexture("../../../data/Textures/Buildings/Concrete_Blocks_004_basecolor.jpg");
plane_normal = CreateTexture("../../../data/Textures/Buildings/Concrete_Blocks_004_normal.jpg");
plane_metallic = CreateTexture("../../../data/Textures/Buildings/Concrete_Blocks_004_height.png");
plane_roughness = CreateTexture("../../../data/Textures/Buildings/Concrete_Blocks_004_roughness.jpg");
plane_ao = CreateTexture("../../../data/Textures/Buildings/Concrete_Blocks_004_ambientOcclusion.jpg");

SetTexture(plane_albedo,plane_mat);
SetNormalMap(plane_normal,plane_mat);
SetMetallicMap(plane_metallic,plane_mat);
SetRoughnessMap(plane_roughness,plane_mat);
SetAOMap(plane_ao,plane_mat);

plane_geometry = CreatePlane();
SetMaterial(plane_geometry,plane_mat);

plane = CreateEntity()
SetName("Plane", plane);
--NewWorldTransform(plane)
--NewLocalTransform(plane)
NewLocalTransformTRS(plane)
NewRenderComponent(plane)

SetMesh(plane, plane_geometry)
SetPositionTRS(plane, {0.0,-0.5,0.0})
SetScaleTRS(plane, {20.0,20.0,20.0})
SetRotationTRS(plane, {-90.0,0.0,0.0})


-- BLOOM--
bloom_mat = CreateDiffuseMaterial();
SetColor(bloom_mat,{1000.0, 0.0, 0.0, 0.0})
SetEmissive(bloom_mat,true)

--TOWERS--
tower_mat = CreatePBRMaterial()
tower_tex_dif = CreateTexture("../../../data/Textures/Buildings/T_building_01_Dif_b.png")
tower_tex_spec = CreateTexture("../../../data/Textures/Buildings/T_building_01_spec.png")
tower_tex_nor = CreateTexture("../../../data/Textures/Buildings/T_building_01_nor.png")
tower_mat2 = CreatePBRMaterial()
tower2_tex_dif = CreateTexture("../../../data/Textures/Buildings/T_building_01_Dif.png")

o_tower_mat = CreatePBRMaterial()
o_tower_tex_dif = CreateTexture("../../../data/Textures/Buildings/T_building_03_Dif.png")
o_tower_tex_spec = CreateTexture("../../../data/Textures/Buildings/T_building_03_spec.png")
o_tower_tex_nor = CreateTexture("../../../data/Textures/Buildings/T_building_03_nor.png")



SetTexture(tower_tex_dif,tower_mat)
SetMetallicMap(tower_tex_spec,tower_mat)
SetNormalMap(tower_tex_nor,tower_mat)

SetTexture(tower2_tex_dif,tower_mat2)
SetMetallicMap(tower_tex_spec,tower_mat2)
SetNormalMap(tower_tex_nor,tower_mat2)


SetTexture(o_tower_tex_dif,o_tower_mat)
SetMetallicMap(o_tower_tex_spec,o_tower_mat)
SetNormalMap(o_tower_tex_nor,o_tower_mat)

tower_geo = CreateObj("../../../data/Models/Buildings/SM_Building_00.obj")
SetMaterial(tower_geo, tower_mat)

tower_geo_2= CreateObj("../../../data/Models/Buildings/SM_Building_00.obj")
SetMaterial(tower_geo_2, tower_mat2)

o_tower_geo = CreateObj("../../../data/Models/Buildings/SM_Building_03.obj")
SetMaterial(o_tower_geo, o_tower_mat)




tower_array = {}

for i=1,5 do
  for j=1,5 do
    tower_array[j * 5 + i] = CreateEntity()
    SetName("Build ".. i, tower_array[j * 5 + i])
    NewWorldTransform(tower_array[j * 5 + i])
    NewRenderComponent(tower_array[j * 5 + i])
    rand_n = math.random(1,3)

    if rand_n == 1 then
      SetMesh(tower_array[j * 5 + i], tower_geo)
    elseif rand_n == 2 then
      SetMesh(tower_array[j * 5 + i], tower_geo_2)
    elseif rand_n == 3 then
      SetMesh(tower_array[j * 5 + i], o_tower_geo)
    end

      SetWorld(tower_array[j * 5 + i], {i * 4.0 + (-12.0) ,-0.5,  j * 5 + (-12)}, {0.0,90.0,0.0}, {0.0015,0.0015,0.0015})
  end
end



--SPHERES--
sphere_mat = CreatePBRMaterial();

--sphere_albedo = CreateTexture("../../../data/Textures/Metals/rustediron2_basecolor.png");
--sphere_normal = CreateTexture("../../../data/Textures/Metals/rustediron2_normal.png");
--sphere_metallic = CreateTexture("../../../data/Textures/Metals/rustediron2_metallic.png");
--sphere_roughness = CreateTexture("../../../data/Textures/Metals/rustediron2_roughness.png");
--sphere_ao = CreateTexture("../../../data/Textures/Metals/rustediron2_metallic.png");

sphere_albedo = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic4-alb.png");
sphere_normal = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-normal.png");
sphere_metallic = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-metal.png");
sphere_roughness = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-rough.png");
sphere_ao = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-ao.png");

--sphere_albedo = CreateTexture("../../../data/Textures/Metals/gold-scuffed_basecolor-boosted.png");
--sphere_normal = CreateTexture("../../../data/Textures/Metals/gold-scuffed_normal.png");
--sphere_metallic = CreateTexture("../../../data/Textures/Metals/gold-scuffed_metallic.png");
--sphere_roughness = CreateTexture("../../../data/Textures/Metals/gold-scuffed_roughness.png");
--sphere_ao = CreateTexture("../../../data/Textures/Metals/gold-scuffed_metallic.png");

SetTexture(sphere_albedo,sphere_mat);
SetNormalMap(sphere_normal,sphere_mat);
SetMetallicMap(sphere_metallic,sphere_mat);
SetRoughnessMap(sphere_roughness,sphere_mat);
SetAOMap(sphere_ao,sphere_mat);

sphere_geometry = CreateCube();
SetMaterial(sphere_geometry, bloom_mat);

sphere_ent = {}

for i=1, 4 do
  sphere_ent[i] = CreateEntity();
  SetName("Sphere Bloom ".. i, sphere_ent[i])
  NewWorldTransform(sphere_ent[i])
  NewLocalTransform(sphere_ent[i])
  NewLocalTransformTRS(sphere_ent[i])
  SetScaleTRS(sphere_ent[i], {0.5, 0.5,0.5})
  NewRenderComponent(sphere_ent[i])
  SetMesh(sphere_ent[i], sphere_geometry)
end





-- LIGHTS --
directional_light = CreateEntity()
SetName("Directional Light", directional_light);
NewWorldTransform(directional_light)
NewDirectionalLightComponent(directional_light)

SetWorld(directional_light, {0.0,10.0,10.0}, {-45.0,0.0,0.0}, {1.0,1.0,1.0})
SetLightCastShadows(directional_light, true)
SetLightColor(directional_light, {255.0,255.0,255.0,255.0})
SetLightIntensity(directional_light, 2.0);

-- Create a 10 x 10 array

lights_bloom = {}

for i=1,4 do
    lights_bloom[i] = {}


        lights_bloom[i] = CreateEntity();
        SetName("Bloom Light ".. i, lights_bloom[i])
        NewWorldTransform(lights_bloom[i])
        NewLocalTransform(lights_bloom[i])
        --NewLocalTransformTRS(lights_bloom[i])
        NewPointLightComponent(lights_bloom[i])
        SetLocal(lights_bloom[i], {0.0 ,0.0, 0.0}, {0,0.0,0.0}, {1.0,1.0,1.0})
        --SetLightDirection(lights_bloom[i], {0.0,-1.0,0.0})
        --SetLocal(lights_bloom[i], {i * 8.0 + (-12.0) ,5.0,  j * 5 + (-12)},{-90,0.0,0.0}, {1.0,1.0,1.0})
        --SetWorld(lights_bloom[i],{i * 10.0 + 1.0 ,5.0,j * 10.0 + 1.0},{-90,0.0,0.0}, {1.0,1.0,1.0})
        --SetRotationTRS(lights_bloom[i], {-90.0,0.0,0.0})
        --SetLightInnerAngle(lights_bloom[i], math.random(5,20));
        --SetLightOuterAngle(lights_bloom[i],  math.random(30,50))
        SetLightIntensity(lights_bloom[i],100.0)
        SetLightCastShadows(lights_bloom[i], false)
        SetLightColor(lights_bloom[i], {255.0 ,0 ,0 ,255.0})
        SetChild(lights_bloom[i], sphere_ent[i])

end

lights_spot = {}

for i=1,3 do
    lights_spot[i] = {}

        lights_spot[i] = CreateEntity();
        SetName("Spot Light ".. i, lights_spot[i])
        NewWorldTransform(lights_spot[i])
        NewLocalTransform(lights_spot[i])
        NewLocalTransformTRS(lights_spot[i])
        NewSpotLightComponent(lights_spot[i])
        SetLightDirection(lights_spot[i], {0.0,-1.0,0.0})
        --SetLocal(lights_spot[i], {i * 8.0 + (-12.0) ,5.0,  j * 5 + (-12)},{-90,0.0,0.0}, {1.0,1.0,1.0})
        --SetWorld(lights_spot[i],{i * 10.0 + 1.0 ,5.0,j * 10.0 + 1.0},{-90,0.0,0.0}, {1.0,1.0,1.0})
        SetRotationTRS(lights_spot[i], {-90.0,0.0,0.0})
        SetPositionTRS(lights_spot[i], {i * 5.0, 2.0, 0.0})
        SetLightInnerAngle(lights_spot[i], 20.0);
        SetLightOuterAngle(lights_spot[i],  50.0)
        SetLightIntensity(lights_spot[i],100.0)
        SetLightCastShadows(lights_spot[i], false)
        SetLightColor(lights_spot[i], {255.0 ,255.0 ,255.0 ,255.0})


end

tick = 0
