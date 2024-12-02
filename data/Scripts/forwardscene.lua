-- PLANE --

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
NewWorldTransform(plane)
NewLocalTransform(plane)
NewLocalTransformTRS(plane)
NewRenderComponent(plane)

SetMesh(plane, plane_geometry)
SetPositionTRS(plane, {0.0,-0.5,0.0})
SetScaleTRS(plane, {50.0,50.0,50.0})
SetRotationTRS(plane, {-90.0,0.0,0.0})



--SPHERES--

sphere_mat = CreatePBRMaterial();

sphere_albedo = CreateTexture("../../../data/Textures/Wood/hardwood-brown-planks-albedo.png");
sphere_normal = CreateTexture("../../../data/Textures/Wood/hardwood-brown-planks-normal-ogl.png");
sphere_metallic = CreateTexture("../../../data/Textures/Wood/hardwood-brown-planks-metallic.png");
sphere_roughness = CreateTexture("../../../data/Textures/Wood/hardwood-brown-planks-roughness.png");
sphere_ao = CreateTexture("../../../data/Textures/Wood/hardwood-brown-planks-ao.png");

SetTexture(sphere_albedo,sphere_mat);
SetNormalMap(sphere_normal,sphere_mat);
SetMetallicMap(sphere_metallic,sphere_mat);
SetRoughnessMap(sphere_roughness,sphere_mat);
SetAOMap(sphere_ao,sphere_mat);

sphere_geometry = CreateSphere();
SetMaterial(sphere_geometry,sphere_mat);

sphere_array = {}
for i = 1, 3, 1 do
    sphere_array[i] = {}
    for j = 1, 3, 1 do
        sphere_array[i][j] = CreateEntity();
        NewWorldTransform(sphere_array[i][j])
        NewLocalTransform(sphere_array[i][j])
        NewLocalTransformTRS(sphere_array[i][j])
        NewRenderComponent(sphere_array[i][j])

        SetMesh(sphere_array[i][j], sphere_geometry)
        SetPositionTRS(sphere_array[i][j], {i * 5.0 + 2.0,0.0,j * 5.0 + 2.0})
        SetScaleTRS(sphere_array[i][j], {1.0,1.0,1.0})
        SetRotationTRS(sphere_array[i][j], {0.0,0.0,0.0})
    end
end




-- LIGHTS --
directional_light = CreateEntity()
NewWorldTransform(directional_light)
NewDirectionalLightComponent(directional_light)

SetWorld(directional_light, {0.0,10.0,-2.0}, {-45.0,0.0,0.0}, {1.0,1.0,1.0})
SetLightCastShadows(directional_light, true)
SetLightColor(directional_light, {255.0,255.0,255.0,255.0})

-- Create a 10 x 10 array
math.randomseed(os.time()) -- random initialize
math.random(); math.random(); math.random() -- warming up
lights_array = {}
for i = 1, 3, 1 do
    lights_array[i] = {}
    for j = 1, 3, 1 do
        lights_array[i][j] = CreateEntity();
        NewWorldTransform(lights_array[i][j]);
        NewSpotLightComponent(lights_array[i][j]);
        SetLightDirection(lights_array[i][j], {0.0,-1.0,0.0})
        SetWorld(lights_array[i][j],{i * 5.0,0.0,j * 5.0},{0.0,math.random(0,360),0.0}, {1.0,1.0,1.0})
        SetLightInnerAngle(lights_array[i][j], math.random(5,20));
        SetLightOuterAngle(lights_array[i][j],  math.random(30,50))
        SetLightCastShadows(lights_array[i][j], true)
        SetLightColor(lights_array[i][j], {math.random(0,255) * 100,math.random(0,255) * 100,math.random(0,255) * 100,255.0})
    end
end
