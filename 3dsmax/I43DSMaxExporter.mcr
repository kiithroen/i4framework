macroScript I43DSMaxExporter category:"I4Framework"
(
	fn getDepthStr depth =
	(
		local str = ""
		for i = 1 to depth do (str += "\t")
		return str
	)
	
	fn getLocalTM obj =
	(
		local localTM = copy obj.transform
		if obj.parent != undefined then
		(
			localTM = obj.transform*(inverse obj.parent.transform)
		)
		return localTM
	)
	
	fn isBone obj =
	(
		return (classof obj == Biped_Object) or (classof obj == Bone) or (classof obj == BoneGeometry) or (classof obj == Dummy) 
	)
	
	fn getRootObjects =
	(
		local rootObjects = for o in geometry where o.parent == undefined and not (isBone o) collect o
		return rootObjects
	)
	
	fn writeTM f n tm depth =
	(
		local depthStr = getDepthStr depth
		
		format "%<%>\n" depthStr n to:f
		format "%\t<val>% % %</val>\n" depthStr tm.row1.x tm.row1.z tm.row1.y to:f
		format "%\t<val>% % %</val>\n" depthStr tm.row3.x tm.row3.z tm.row3.y to:f
		format "%\t<val>% % %</val>\n" depthStr tm.row2.x tm.row2.z tm.row2.y to:f
		format "%\t<val>% % %</val>\n" depthStr tm.row4.x tm.row4.z tm.row4.y to:f
		format "%</%>\n" depthStr n to:f
	)
	
	fn writeVertex f tmesh obj depth =
	(
		local depthStr = getDepthStr depth
		
		local numVerts = tmesh.numverts
		local TM = obj.transform
		local invTM = inverse TM
		
		format "%<vertex count=\"%\">\n" depthStr numVerts to:f
		
		for i = 1 to numVerts do
		(
			local vtx = in coordsys world getVert tmesh i
			vtx = vtx * invTM
			format "%\t<val x=\"%\" y=\"%\" z=\"%\"/>\n" depthStr vtx.x vtx.z vtx.y to:f
		)
		format "%</vertex>\n" depthStr to:f
	)
	
	fn writeNormal f tmesh depth =
	(
		local depthStr = getDepthStr depth
		
		local numVerts = tmesh.numverts
		
		format "%<normal count=\"%\">\n" depthStr numVerts to:f
		
		for i = 1 to numVerts do
		(
			local vtx = normalize (getNormal tmesh i)
			format "%\t<val x=\"%\" y=\"%\" z=\"%\"/>\n" depthStr vtx.x vtx.z vtx.y to:f
		)
		format "%</normal>\n" depthStr to:f
	)
	
	fn writeIndex f tmesh depth =
	(
		local depthStr = getDepthStr depth
		
		local numFace = tmesh.numfaces
		
		format "%<index count=\"%\">\n" depthStr numFace to:f
		
		for i = 1 to numFace do
		(
			local idx = getFace tmesh i
			format "%\t<val x=\"%\" y=\"%\" z=\"%\"/>\n" depthStr ((idx.x - 1) as integer ) ((idx.z - 1) as integer) ((idx.y - 1) as integer) to:f
		)
		format "%</index>\n" depthStr to:f
	)
	
	fn writeTexUV f tmesh depth =
	(
		local depthStr = getDepthStr depth
		
		local numTVerts = tmesh.numtverts
		
		format "%<texUV count=\"%\">\n" depthStr numTVerts to:f
		
		for i = 1 to numTVerts do
		(
			local uv = getTVert tmesh i
			format "%\t<val u=\"%\" v=\"%\"/>\n" depthStr (uv.x) (1.0 - uv.y) to:f
		)
		format "%</texUV>\n" depthStr to:f
	)
	
	fn writeTexIndex f tmesh depth =
	(
		local depthStr = getDepthStr depth
		
		local numTFace = tmesh.numfaces
		
		format "%<texIndex count=\"%\">\n" depthStr numTFace to:f
		
		for i = 1 to numTFace do
		(
			local idx = getTVFace tmesh i
			format "%\t<val x=\"%\" y=\"%\" z=\"%\"/>\n" depthStr ((idx.x - 1) as integer ) ((idx.z - 1) as integer) ((idx.y - 1) as integer) to:f
		)
		format "%</texIndex>\n" depthStr to:f
	)
		
	fn writeMesh f obj depth =
	(
		local depthStr = getDepthStr depth
		local localTM = getLocalTM obj	
		local tmesh = snapshotAsMesh obj
		
		format "%<mesh name=\"%\" parent_name=\"\">\n" depthStr obj.name to:f
		
		writeTM f "localTM" localTM (depth + 1)
		writeVertex f tmesh obj (depth + 1)
		writeNormal f tmesh (depth + 1)
		writeIndex f tmesh (depth + 1)
		writeTexUV f tmesh (depth + 1)
		writeTexIndex f tmesh (depth + 1)
		
		format "%</mesh>\n" depthStr to:f
	)
	
	
	fn ExportRecursive f obj depth =
	(
		writeMesh f obj depth
		
		for child in obj.children do
		(
			ExportRecursive f child depth
		)
	)
	
	local exportName = "test"
	local exportType = "mesh"
	local exportFullName = "d:\\" + exportName + "." + exportType + ".xml"
	local version = 100
	
	local rootObjects = getRootObjects()

	local f = createFile exportFullName
	
	format "<model name=\"%\">\n" exportName to:f
	
	format "\t<version>%</version>\n" version to:f
	
	for obj in rootObjects do
	(
		ExportRecursive f obj 1
	)
	
	format "</model>" to:f
	
	close f
	
	print "success"
)