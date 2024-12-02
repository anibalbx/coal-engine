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


--SPHERES--
sphere_mat = CreatePBRMaterial();
cube_mat = CreatePBRMaterial();

sphere_albedo = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic4-alb.png");
sphere_normal = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-normal.png");
sphere_metallic = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-metal.png");
sphere_roughness = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-rough.png");
sphere_ao = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-ao.png");

SetTexture(sphere_albedo,sphere_mat);
SetNormalMap(sphere_normal,sphere_mat);
SetMetallicMap(sphere_metallic,sphere_mat);
SetRoughnessMap(sphere_roughness,sphere_mat);
SetAOMap(sphere_ao,sphere_mat);

--CUBE--
cube_albedo = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic3-alb.png");
cube_normal = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-normal.png");
cube_metallic = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-metal.png");
cube_roughness = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-rough.png");
cube_ao = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-ao.png");

SetTexture(cube_albedo,cube_mat);
SetNormalMap(cube_normal,cube_mat);
SetMetallicMap(cube_metallic,cube_mat);
SetRoughnessMap(cube_roughness,cube_mat);
SetAOMap(cube_ao,cube_mat);

sphere_geometry = CreateSphere();
cube_geometry = CreateCube();
SetMaterial(sphere_geometry,sphere_mat);
SetMaterial(cube_geometry,cube_mat);


sphere_id = CreateEntity();
SetName("Sphere",sphere_id)
NewWorldTransform(sphere_id)
NewLocalTransform(sphere_id)
NewRenderComponent(sphere_id)
SetMesh(sphere_id, sphere_geometry)
NewLocalTransformTRS(sphere_id)
SetPositionTRS(sphere_id, {0.0 ,1.0, 0.0})
SetScaleTRS(sphere_id, {1.0,1.0,1.0})
SetRotationTRS(sphere_id, {0.0,0.0,0.0})

cube_id = CreateEntity();
SetName("Cube",cube_id)
NewWorldTransform(cube_id)
NewLocalTransform(cube_id)
NewRenderComponent(cube_id)
SetMesh(cube_id, cube_geometry)
NewLocalTransformTRS(cube_id)
SetPositionTRS(cube_id, {2.0 ,0.5, 0.0})
SetScaleTRS(cube_id, {1.0,1.0,1.0})
SetRotationTRS(cube_id, {0.0,0.0,0.0})


-- Create a 10 x 10 array
math.randomseed(os.time()) -- random initialize
math.random(); math.random(); math.random() -- warming up
lights_array = {}

for i = 0, 1, 1 do
    lights_array[i] = {}
    lights_array[i] = CreateEntity();
    SetName("Spot Light ".. i + 1, lights_array[i])
    NewWorldTransform(lights_array[i]);
    NewSpotLightComponent(lights_array[i]);
    SetLightDirection(lights_array[i], {0.0,-1.0,0.0})
    SetWorld(lights_array[i],{0.0,2.0,5.0},{0.0,math.random(0,360),0.0}, {1.0,1.0,1.0})
    SetLightInnerAngle(lights_array[i], math.random(5,20));
    SetLightOuterAngle(lights_array[i],  math.random(30,50))
    SetLightIntensity(lights_array[i],100.0);
    SetLightCastShadows(lights_array[i], true)
    SetLightColor(lights_array[i], {255.0 ,255.0 ,255.0,255.0})
end
