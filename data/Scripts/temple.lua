-- PLANE --

teste = CreateEntity()

tick = 0

plane_mat = CreatePBRMaterial();

plane_albedo = CreateTexture("../../../data/Textures/Wood/hardwood-brown-planks-albedo.png");
plane_normal = CreateTexture("../../../data/Textures/Wood/hardwood-brown-planks-normal-ogl.png");
plane_metallic = CreateTexture("../../../data/Textures/Wood/hardwood-brown-planks-metallic.png");
plane_roughness = CreateTexture("../../../data/Textures/Wood/hardwood-brown-planks-roughness.png");
plane_ao = CreateTexture("../../../data/Textures/Wood/hardwood-brown-planks-ao.png");

SetTexture(plane_albedo,plane_mat);
SetNormalMap(plane_normal,plane_mat);
SetMetallicMap(plane_metallic,plane_mat);
SetRoughnessMap(plane_roughness,plane_mat);
SetAOMap(plane_ao,plane_mat);

plane_geometry = CreatePlane();
SetMaterial(plane_geometry,plane_mat);

plane = {}

for i = 0, 1, 1 do
  plane[i] = CreateEntity()
  SetName("Plane", plane[i]);
  NewWorldTransform(plane[i])
  NewLocalTransform(plane[i])
  NewLocalTransformTRS(plane[i])
  NewRenderComponent(plane[i])
  SetMesh(plane[i], plane_geometry)
  SetPositionTRS(plane[i], {0.0,0.0,0.0})
  SetScaleTRS(plane[i], {5.0,5.0,5.0})
  SetRotationTRS(plane[i], {-90.0,0.0,0.0})
end

-- BLOOM--

bloom_mat = CreateDiffuseMaterial();
SetColor(bloom_mat,{255.0, 0.0, 0.0, 0.0})
SetEmissive(bloom_mat,false)


--TEMPLE--
temple_mat = CreatePBRMaterial();

temple_albedo = CreateTexture("../../../data/Textures/Buildings/Japanese_Temple_Paint2_Japanese_Shrine_Mat_AlbedoTransparency.png");
temple_normal = CreateTexture("../../../data/Textures/Buildings/Japanese_Temple_Paint2_Japanese_Shrine_Mat_Normal.png");
temple_metallic = CreateTexture("../../../data/Textures/Buildings/Japanese_Temple_Paint2_Japanese_Shrine_Mat_MetallicSmoothness.png");
temple_roughness = CreateTexture("../../../data/Textures/Buildings/Japanese_Temple_Paint2_Japanese_Shrine_Mat_Roughness.png");
temple_ao = CreateTexture("../../../data/Textures/Buildings/Japanese_Temple_Paint2_Japanese_Shrine_Mat_AO.png");

SetTexture(temple_albedo,temple_mat);
SetNormalMap(temple_normal,temple_mat);
SetMetallicMap(temple_metallic,temple_mat);
SetRoughnessMap(temple_roughness,temple_mat);
SetAOMap(temple_ao,temple_mat);

temple_geometry = CreateObj("../../../data/Models/Buildings/Japanese_Temple.obj")
SetMaterial(temple_geometry, temple_mat)

-- sphere_albedo = CreateTexture("../../../data/Textures/Metals/rustediron2_basecolor.png");
-- sphere_normal = CreateTexture("../../../data/Textures/Metals/rustediron2_normal.png");
-- sphere_metallic = CreateTexture("../../../data/Textures/Metals/rustediron2_metallic.png");
-- sphere_roughness = CreateTexture("../../../data/Textures/Metals/rustediron2_roughness.png");
-- sphere_ao = CreateTexture("../../../data/Textures/Metals/rustediron2_metallic.png");



--SetMaterial(sphere_geometry,bloom_mat);

temple_id = CreateEntity();
SetName("Temple",temple_id)
NewWorldTransform(temple_id)
NewLocalTransform(temple_id)
NewRenderComponent(temple_id)

SetMesh(temple_id, temple_geometry)

NewLocalTransformTRS(temple_id)
SetPositionTRS(temple_id, {0.0 ,2.0, 0.0})
SetScaleTRS(temple_id, {0.1,0.1,0.1})
SetRotationTRS(temple_id, {0.0,0.0,0.0})

--LIGHTS --
directional_light = CreateEntity()
SetName("Directional Light", directional_light);
NewWorldTransform(directional_light)
NewDirectionalLightComponent(directional_light)

SetWorld(directional_light, {0.0,10.0,15.0}, {-45.0,0.0,0.0}, {1.0,1.0,1.0})
SetLightCastShadows(directional_light, true)
SetLightColor(directional_light, {255.0,255.0,255.0,255.0})
SetLightIntensity(directional_light, 5.0);


-- Create a 10 x 10 array
math.randomseed(os.time()) -- random initialize
math.random(); math.random(); math.random() -- warming up
lights_array = {}

for i = 0, 1, 1 do
    lights_array[i] = {}
    lights_array[i] = CreateEntity();
    SetName("Point Light ".. i + 1, lights_array[i])
    NewWorldTransform(lights_array[i]);
    NewPointLightComponent(lights_array[i]);
    SetLightDirection(lights_array[i], {0.0,-1.0,0.0})
    SetWorld(lights_array[i],{0.0,2.0,5.0},{0.0,math.random(0,360),0.0}, {1.0,1.0,1.0})
    SetLightInnerAngle(lights_array[i], math.random(5,20));
    SetLightOuterAngle(lights_array[i],  math.random(30,50))
    SetLightIntensity(lights_array[i],100.0);
    SetLightCastShadows(lights_array[i], true)
    SetLightColor(lights_array[i], {255.0 ,255.0 ,255.0,255.0})
end
