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
  SetScaleTRS(plane[i], {20.0,20.0,20.0})
  SetRotationTRS(plane[i], {-90.0,0.0,0.0})
end




-- BLOOM--

bloom_mat_white = CreateDiffuseMaterial();
SetColor(bloom_mat_white,{255.0, 255.0, 255.0, 255.0})
SetEmissive(bloom_mat_white,true)

bloom_mat_red= CreateDiffuseMaterial();
SetColor(bloom_mat_red,{255.0, 0.0, 0.0, 255.0})
SetEmissive(bloom_mat_red,true)

bloom_mat_yellow = CreateDiffuseMaterial();
SetColor(bloom_mat_yellow,{255.0, 255.0, 0.0, 255.0})
SetEmissive(bloom_mat_yellow,true)

bloom_mat_green = CreateDiffuseMaterial();
SetColor(bloom_mat_green,{0.0, 255.0, 0.0, 255.0})
SetEmissive(bloom_mat_green,true)

bloom_mat_blue = CreateDiffuseMaterial();
SetColor(bloom_mat_blue,{0.0, 0.0, 255.0, 255.0})
SetEmissive(bloom_mat_blue,true)

cube_geometry_white = CreateCube();
SetMaterial(cube_geometry_white,bloom_mat_white);

cube_geometry_red = CreateCube();
SetMaterial(cube_geometry_red,bloom_mat_red);

cube_geometry_yellow = CreateCube();
SetMaterial(cube_geometry_yellow,bloom_mat_yellow);

cube_geometry_green= CreateCube();
SetMaterial(cube_geometry_green,bloom_mat_green);

cube_geometry_blue = CreateCube();
SetMaterial(cube_geometry_blue,bloom_mat_blue);


cube_white_id = CreateEntity();
SetName("White Cube",cube_white_id)
NewWorldTransform(cube_white_id)
NewLocalTransform(cube_white_id)
NewRenderComponent(cube_white_id)

SetMesh(cube_white_id, cube_geometry_white)

NewLocalTransformTRS(cube_white_id)
SetPositionTRS(cube_white_id, {0.0 ,2.0, 0.0})
SetScaleTRS(cube_white_id, {1.0,1.0,1.0})
SetRotationTRS(cube_white_id, {0.0,0.0,0.0})


cube_red_id = CreateEntity();
SetName("Red Cube",cube_red_id)
NewWorldTransform(cube_red_id)
NewLocalTransform(cube_red_id)
NewRenderComponent(cube_red_id)

SetMesh(cube_red_id, cube_geometry_red)

NewLocalTransformTRS(cube_red_id)
SetPositionTRS(cube_red_id, {0.0 ,2.0, 0.0})
SetScaleTRS(cube_red_id, {1.0,1.0,1.0})
SetRotationTRS(cube_red_id, {0.0,0.0,0.0})

cube_yellow_id = CreateEntity();
SetName("Yellow Cube",cube_yellow_id)
NewWorldTransform(cube_yellow_id)
NewLocalTransform(cube_yellow_id)
NewRenderComponent(cube_yellow_id)

SetMesh(cube_yellow_id, cube_geometry_yellow)

NewLocalTransformTRS(cube_yellow_id)
SetPositionTRS(cube_yellow_id, {0.0 ,2.0, 0.0})
SetScaleTRS(cube_yellow_id, {1.0,1.0,1.0})
SetRotationTRS(cube_yellow_id, {0.0,0.0,0.0})

cube_green_id = CreateEntity();
SetName("Green Cube",cube_green_id)
NewWorldTransform(cube_green_id)
NewLocalTransform(cube_green_id)
NewRenderComponent(cube_green_id)

SetMesh(cube_green_id, cube_geometry_green)

NewLocalTransformTRS(cube_green_id)
SetPositionTRS(cube_green_id, {0.0 ,2.0, 0.0})
SetScaleTRS(cube_green_id, {1.0,1.0,1.0})
SetRotationTRS(cube_green_id, {0.0,0.0,0.0})

cube_blue_id = CreateEntity();
SetName("Blue Cube",cube_blue_id)
NewWorldTransform(cube_blue_id)
NewLocalTransform(cube_blue_id)
NewRenderComponent(cube_blue_id)

SetMesh(cube_blue_id, cube_geometry_blue)

NewLocalTransformTRS(cube_blue_id)
SetPositionTRS(cube_blue_id, {0.0 ,2.0, 0.0})
SetScaleTRS(cube_blue_id, {1.0,1.0,1.0})
SetRotationTRS(cube_blue_id, {0.0,0.0,0.0})


-- Create a 10 x 10 array
math.randomseed(os.time()) -- random initialize
math.random(); math.random(); math.random() -- warming up
lights_array = {}

for i = 0, 4, 1 do
    lights_array[i] = {}
    lights_array[i] = CreateEntity();
    SetName("Point Light ".. i + 1, lights_array[i])
    NewWorldTransform(lights_array[i]);
    NewPointLightComponent(lights_array[i]);
    SetLightDirection(lights_array[i], {0.0,-1.0,0.0})
    SetWorld(lights_array[i],{0.0,2.0,5.0},{0.0,math.random(0,360),0.0}, {1.0,1.0,1.0})
    SetLightInnerAngle(lights_array[i], math.random(5,20));
    SetLightOuterAngle(lights_array[i],  math.random(30,50))
    SetLightIntensity(lights_array[i],0.0);
    SetLightCastShadows(lights_array[i], true)
    SetLightColor(lights_array[i], {255.0 ,255.0 ,255.0,255.0})
end
