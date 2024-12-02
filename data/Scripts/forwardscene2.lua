-- PLANE --

teste = CreateEntity()



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

plane = CreateEntity()
SetName("Plane", plane);
NewWorldTransform(plane)
NewLocalTransform(plane)
NewLocalTransformTRS(plane)
NewRenderComponent(plane)

SetMesh(plane, plane_geometry)
SetPositionTRS(plane, {0.0,-0.5,0.0})
SetScaleTRS(plane, {20.0,20.0,20.0})
SetRotationTRS(plane, {-90.0,0.0,0.0})


-- BLOOM--
bloom_mat = CreateDiffuseMaterial();
SetColor(bloom_mat,{255.0, 0.0, 0.0, 0.0})
SetEmissive(bloom_mat,true)


--SPHERES--
sphere_mat = CreatePBRMaterial();

--sphere_albedo = CreateTexture("../../../data/Textures/Metals/rustediron2_basecolor.png");
--sphere_normal = CreateTexture("../../../data/Textures/Metals/rustediron2_normal.png");
--sphere_metallic = CreateTexture("../../../data/Textures/Metals/rustediron2_metallic.png");
--sphere_roughness = CreateTexture("../../../data/Textures/Metals/rustediron2_roughness.png");
--sphere_ao = CreateTexture("../../../data/Textures/Metals/rustediron2_metallic.png");

-- sphere_albedo = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic4-alb.png");
-- sphere_normal = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-normal.png");
-- sphere_metallic = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-metal.png");
-- sphere_roughness = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-rough.png");
-- sphere_ao = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-ao.png");

sphere_albedo = CreateTexture("../../../data/Textures/Metals/gold-scuffed_basecolor-boosted.png");
sphere_normal = CreateTexture("../../../data/Textures/Metals/gold-scuffed_normal.png");
sphere_metallic = CreateTexture("../../../data/Textures/Metals/gold-scuffed_metallic.png");
sphere_roughness = CreateTexture("../../../data/Textures/Metals/gold-scuffed_roughness.png");
sphere_ao = CreateTexture("../../../data/Textures/Metals/gold-scuffed_metallic.png");

SetTexture(sphere_albedo,sphere_mat);
SetNormalMap(sphere_normal,sphere_mat);
SetMetallicMap(sphere_metallic,sphere_mat);
SetRoughnessMap(sphere_roughness,sphere_mat);
SetAOMap(sphere_ao,sphere_mat);

sphere_geometry = CreateSphere();
SetMaterial(sphere_geometry,bloom_mat);
--SetMaterial(sphere_geometry,bloom_mat);

sphere_array = {}

for i = 0, 1, 1 do
    sphere_array[i] = {}
    for j = 0, 1, 1 do

        sphere_array[i][j] = CreateEntity();
        SetName("Sphere ".. (j + i * 3) + 1, sphere_array[i][j]);
        NewWorldTransform(sphere_array[i][j])
        NewLocalTransform(sphere_array[i][j])

        NewRenderComponent(sphere_array[i][j])

        SetMesh(sphere_array[i][j], sphere_geometry)

        NewLocalTransformTRS(sphere_array[i][j])
        SetPositionTRS(sphere_array[i][j], {i * 5.0,0.5,j * 5.0})
        SetScaleTRS(sphere_array[i][j], {1.0,1.0,1.0})
        SetRotationTRS(sphere_array[i][j], {0.0,0.0,0.0})
    end
end

-- LIGHTS --



-- Create a 10 x 10 array
math.randomseed(os.time()) -- random initialize
math.random(); math.random(); math.random() -- warming up
lights_array = {}

for i = 0, 1, 1 do
    lights_array[i] = {}
    for j = 0, 1, 1 do

        lights_array[i][j] = CreateEntity();
        SetName("Spot Light ".. (j + i * 2) + 1, lights_array[i][j])
        NewWorldTransform(lights_array[i][j]);
        NewPointLightComponent(lights_array[i][j]);
        SetLightDirection(lights_array[i][j], {0.0,-1.0,0.0})
        SetWorld(lights_array[i][j],{i * 10.0 + 1.0 ,2.0,j * 10.0 + 1.0},{0.0,math.random(0,360),0.0}, {1.0,1.0,1.0})
        SetLightInnerAngle(lights_array[i][j], math.random(5,20));
        SetLightOuterAngle(lights_array[i][j],  math.random(30,50))
        SetLightIntensity(lights_array[i][j],1000.0);
        SetLightCastShadows(lights_array[i][j], true)
        SetLightColor(lights_array[i][j], {255 ,255 ,255,255.0})
    end
end

tick = 0
