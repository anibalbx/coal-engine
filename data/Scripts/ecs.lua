--Create Material Diffuse & texture




mymattwo = CreateDiffuseMaterial()
--mytexttwo = CreateTexture("../../../data/android.png")
--SetTexture(mytexttwo,mymattwo);
SetColor(mymattwo,{255.0, 0.0, 0.0, 0.0})
SetEmissive(mymattwo,true)

--PBR

pbrmat = CreatePBRMaterial()

-- albedo_texture = CreateTexture("../../../data/Textures/Metals/rustediron2_basecolor.png")
-- normal_map_texture = CreateTexture("../../../data/Textures/Metals/rustediron2_normal.png")
-- metallic_map_texture = CreateTexture("../../../data/Textures/Metals/rustediron2_metallic.png")
-- roughness_map_texture = CreateTexture("../../../data/Textures/Metals/rustediron2_roughness.png")

--albedo_texture = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic4-alb.png")
--normal_map_texture = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-normal.png")
--metallic_map_texture = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-metal.png")
--roughness_map_texture = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-rough.png")
--ao_map_texture = CreateTexture("../../../data/Textures/Plastic/scuffed-plastic-ao.png")

--albedo_texture = CreateTexture("../../../data/Textures/ToonTeddy.jpg")
albedo_texture = CreateTexture("../../../data/Textures/Bird/material0_basecolor.jpg")
--normal_map_texture = CreateTexture("../../../data/Textures/BearNormalMap.png")
normal_map_texture = CreateTexture("../../../data/Textures/Bird/material0_normal.jpg")
ao_map_texture = CreateTexture("../../../data/Textures/Bird/material0_occlusion.jpg")


SetTexture(albedo_texture, pbrmat)
SetNormalMap(normal_map_texture, pbrmat)
--SetMetallicMap(metallic_map_texture, pbrmat)
--SetRoughnessMap(roughness_map_texture, pbrmat)
SetAOMap(ao_map_texture, pbrmat)

goldmat = CreatePBRMaterial();
gold_albedo = CreateTexture("../../../data/Textures/Metals/gold-scuffed_basecolor-boosted.png")
gold_normal = CreateTexture("../../../data/Textures/Metals/gold-scuffed_normal.png")
gold_metallic = CreateTexture("../../../data/Textures/Metals/gold-scuffed_metallic.png")
gold_ao = CreateTexture("../../../data/Textures/Metals/gold-scuffed_metallic.png");
gold_roughness = CreateTexture("../../../data/Textures/Metals/gold-scuffed_roughness.png")


SetTexture(gold_albedo, goldmat)
SetNormalMap(gold_normal, goldmat)
SetMetallicMap(gold_metallic, goldmat)
SetRoughnessMap(gold_roughness, goldmat)
SetAOMap(gold_ao, goldmat);


--Create Mesh

mycube = CreateCube()
--SetMaterial(mycube, mymat)
SetMaterial(mycube, goldmat)

othercube = CreateCube()
SetMaterial(othercube, mymattwo)

somecube = CreateCube()

teapot = CreateObj("../../../data/Models/rapid.obj")
SetMaterial(teapot, pbrmat)

--Create Entity

myentity = CreateEntity()
NewWorldTransform(myentity)
NewLocalTransform(myentity)
NewLocalTransformTRS(myentity)
NewRenderComponent(myentity)

SetMesh(myentity, teapot)
SetPositionTRS(myentity, {-2.0,0.5,-1.0})
SetScaleTRS(myentity, {0.001,0.001,0.001})
SetRotationTRS(myentity, {0.0,0.0,0.0})

myentity2 = CreateEntity()
NewWorldTransform(myentity2)
NewLocalTransform(myentity2)
NewLocalTransformTRS(myentity2)
NewRenderComponent(myentity2)

SetMesh(myentity2, mycube)
--SetPositionTRS(myentity2, {2.0,0.0,-1.0})
SetPositionTRS(myentity2, {2.0,0.0,0.0})
SetScaleTRS(myentity2, {1.0,1.0,1.0})
SetRotationTRS(myentity2, {0.0,0.0,0.0})

--Light entity
lightent = CreateEntity()
NewWorldTransform(lightent)
NewSpotLightComponent(lightent)
--NewDirectionalLightComponent(lightent)

SetWorld(lightent, {0.0,0.0,1.0}, {0.0,0.0,0.0}, {1.0,1.0,1.0})
SetLightInnerAngle(lightent, 10.0)
SetLightOuterAngle(lightent, 40.0)
SetLightDirection(lightent, {0.0,-1.0,0.0})
SetLightCastShadows(lightent, true)
SetLightColor(lightent, {25500.0,25500.0,25500.0,25500.0})


lightent3 = CreateEntity()
NewWorldTransform(lightent3)
NewSpotLightComponent(lightent3)
--NewDirectionalLightComponent(lightent)

SetWorld(lightent3, {0.0,0.0,3.0}, {0.0,0.0,0.0}, {1.0,1.0,1.0})
SetLightInnerAngle(lightent3, 10.0)
SetLightOuterAngle(lightent3, 40.0)
SetLightDirection(lightent3, {0.0,-1.0,0.0})
SetLightCastShadows(lightent3, true)
SetLightColor(lightent3, {25500.0,25500.0,25500.0,25500.0})

lightent2 = CreateEntity()
NewWorldTransform(lightent2)
--NewSpotLightComponent(lightent2)
NewDirectionalLightComponent(lightent2)

SetWorld(lightent2, {0.0,10.0,-2.0}, {-45.0,0.0,0.0}, {1.0,1.0,1.0})
SetLightCastShadows(lightent2, true)
SetLightColor(lightent2, {255.0,255.0,255.0,255.0})

--for x=1,10 do
--  for y=1,10 do
--    aux_ent = CreateEntity()
--    aux_ent = CreateEntity()
--    NewWorldTransform(aux_ent)
--    NewLocalTransform(aux_ent)
--    NewLocalTransformTRS(aux_ent)
--    NewRenderComponent(aux_ent)
--
--    SetMesh(aux_ent, teapot)
--    SetPositionTRS(aux_ent, {x * 2.5,0.5, y * -2.5})
--    SetScaleTRS(aux_ent, {1.0,1.0,1.0})
--    SetRotationTRS(aux_ent, {0.0,0.0,0.0})
--
--  end
--
--end


--otherentity = CreateEntity()
--NewWorldTransform(otherentity)
--NewLocalTransform(otherentity)
--NewLocalTransformTRS(otherentity)
--NewRenderComponent(otherentity)
--SetMesh(otherentity, othercube)
--SetChild(otherentity, myentity)
--SetPositionTRS(otherentity, {2.0,0.0,0.0})
--SetScaleTRS(otherentity, {0.5,0.5,0.5})
--SetRotationTRS(otherentity, {0.0,0.0,0.0})
--SetLocal(otherentity, {2.0,0.0,0.0}, {0.0,0.0,0.0}, {0.5,0.5,0.5})

--childentity = CreateEntity()
--NewWorldTransform(childentity)
--NewLocalTransform(childentity)
--NewRenderComponent(childentity)
--SetMesh(childentity, mycube)
--SetChild(childentity, otherentity)
--SetLocal(childentity, {1.0,0.0,0.0}, {0.0,0.0,0.0}, {0.5,0.5,0.5})

--nochildentity = CreateEntity()
--NewWorldTransform(nochildentity)
--NewLocalTransform(nochildentity)
--NewRenderComponent(nochildentity)
--SetMesh(nochildentity, mycube)
--SetLocal(nochildentity, {1.0,-1.0,-5.0}, {0.0,0.0,0.0}, {0.5,0.5,0.5})
