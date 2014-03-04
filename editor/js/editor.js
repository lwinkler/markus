// Global objects

var maxIdModules = 0;
var xmlModuleTypes = [];
var xmlProject = null;

;(function() {

	// Declarations
	var _initialised = false,
	showConnectionInfo = function(s) {
		$("#list").html(s);
		$("#list").fadeIn({complete:function() { jsPlumb.repaintEverything(); }});
	},	
	hideConnectionInfo = function() {
		$("#list").fadeOut({complete:function() { jsPlumb.repaintEverything(); }});
	},
	connections = [],
	updateConnections = function(conn, remove) {
		if (!remove) connections.push(conn);
		else {
			var idx = -1;
			for (var i = 0; i < connections.length; i++) {
				if (connections[i] == conn) {
					idx = i; break;
				}
			}
			if (idx != -1) connections.splice(idx, 1);
		}
		if (connections.length > 0) {
			var s = "<span><strong>Connections</strong></span><br/><br/><table><tr><th>Scope</th><th>Source</th><th>Target</th></tr>";
			for (var j = 0; j < connections.length; j++) {
				s = s + "<tr><td>" + connections[j].scope + "</td>" + "<td>" + connections[j].sourceId + "</td><td>" + connections[j].targetId + "</td></tr>";
			}
			showConnectionInfo(s);
		} else 
			hideConnectionInfo();
	};
	
	window.markusEditor = {
		init : function() {
		
			// setup jsPlumb defaults.
			jsPlumb.importDefaults({
				DragOptions : { cursor: 'pointer', zIndex:2000 },
				PaintStyle : { strokeStyle:'#666' },
				EndpointStyle : { width:20, height:16, strokeStyle:'#666' },
				Endpoint : "Rectangle",
				Anchors : ["TopCenter", "TopCenter"]
			});												

			// bind to connection/connectionDetached events, and update the list of connections on screen.
			jsPlumb.bind("connection", function(info, originalEvent) {
				updateConnections(info.connection);
			});
			jsPlumb.bind("connectionDetached", function(info, originalEvent) {
				updateConnections(info.connection, true);
			});

			// configure some drop options for use by all endpoints.
			var dropOptions = {
				tolerance:"touch",
				hoverClass:"dropHover",
				activeClass:"dragActive"
			};

			// Configuration of endpoints for jsPlumb
			var colorDefault = "#316b31";
			
			var pointsColor = {
				Image: "#316b31",	
				Objects: "#00f",	
				Debug: "#09098e",
				State: "#cd7f32",
				Event: "#df0101"
			};


			var inputPoint = {
				endpoint:"Rectangle",
				paintStyle:{ fillStyle:colorDefault },
				isSource:false,
				scope:"default",
				connectorStyle:{ strokeStyle:colorDefault, lineWidth:2 },
				maxConnections:1,
				isTarget:true,
				beforeDrop:function(params) { 
					// Connect input with output on xml
					var xmlOutput = $(params.connection.endpoints[0]).data('config');
					var xmlInput  = $(params.dropEndpoint).data('config');

					// if(!! xmlInput || !! xmlOutput)
					// 	return false;
					xmlInput.attr('outputid', xmlOutput.attr('id'));
					xmlInput.attr('moduleid', xmlOutput.parent().parent().attr('id'));

					return true;
				},				
				dropOptions : dropOptions
			};


			var outputPoint = {
				endpoint:["Dot", { radius:10 }],
				paintStyle:{ fillStyle:colorDefault },
				isSource:true,
				scope:"default",
				connectorStyle:{ strokeStyle:colorDefault, lineWidth:2 },
				connector: ["Bezier", { curviness:63 } ],
				// connector: "Straight",
				// connector: "Flowchart",
				maxConnections:99,
				isTarget:false,
				beforeDetach: function(params) { 
					// Connect input with output on xml
					var xmlInput = $(params.suspendedEndpoint).data('config'); 

					// if(!! xmlInput || !! xmlOutput)
					// 	return false;
					xmlInput.removeAttr('outputid');
					xmlInput.removeAttr('moduleid');
					return true;
				},
				dropOptions : dropOptions
			};
			//================================================================================
			// Functions and utilities
			//================================================================================
			function getModuleType(xmlModule) {
				return xmlModule.find('parameters > param[name="class"]').text();
			}
			//-------------------------------------------------------------------------------- 
			// Load an xml file
			//--------------------------------------------------------------------------------
			function loadXML(fileName) {
				var target = "";
				$.ajax({
					type: "GET",
					url : fileName,
					cache: false,
					async: false,
					dataType: "xml",
					success : function (data) {
						target = data;
					},
					error: function(e) { console.log("Error : Failed to load " + fileName + " status:" + e.status)}
				});
				return target;
			}

			//-------------------------------------------------------------------------------- 
			// Parse string and return string
			//--------------------------------------------------------------------------------
			if (window.DOMParser) {
				parseXml = function(xmlStr) {
					return ( new window.DOMParser() ).parseFromString(xmlStr, "text/xml");
				};
			} else if (typeof window.ActiveXObject != "undefined" && new window.ActiveXObject("Microsoft.XMLDOM")) {
				parseXml = function(xmlStr) {
					var xmlDoc = new window.ActiveXObject("Microsoft.XMLDOM");
					xmlDoc.async = "false";
					xmlDoc.loadXML(xmlStr);
					return xmlDoc;
				};
			} else {
				parseXml = function() { return null; }
			}
			//--------------------------------------------------------------------------------
			// Delete everything
			//--------------------------------------------------------------------------------
			function deleteAll(){
				try{
					jsPlumb.detachEveryConnection();
				}
				catch(err){}
				$("#main > .window").remove();
				$("#main > ._jsPlumb_endpoint").remove();
				maxIdModules = 0;

				// Load an empty project
				xmlProject = $(document.implementation.createDocument(null, "application", null)).find('application');
				xmlProject.attr("name", "CustomProject")

				$('#detail').hide();
				$('#explanation').show();
			}
			//--------------------------------------------------------------------------------
			// Create a window to visualize the module with jsPlumb
			//--------------------------------------------------------------------------------
			function createModuleWindow(xmlModule, id, uiobject) {
				var type = getModuleType(xmlModule); 
				var name = xmlModule.attr("name");
				var newWindow = $('<div/>', {
					class:"window",
					id: 'w' + id
				})
				.append('<h3 id="name">' + name + '</h3>')
				.append('<p id="type">' + type + '</p>')
				// .append('<a class="cmdLink hide"   rel="w' + id + '">toggle connections</a><br/>')
				// .append('<p><a class="cmdLink drag"   rel="w' + id + '">disable dragging</a></p>')
				// .append('<p><a class="cmdLink detach" rel="w' + id + '">detach all</a></p>')
				.append('<p><a class="cmdLink delete" rel="w' + id + '">delete</a></p>');

				// Append the module window to main div 
				$("#main").append(newWindow);

				// Position the window if a position was saved
				if(!! uiobject)
					newWindow.offset({
						left: uiobject.attr('x'),
						top: uiobject.attr('y')
					})
				else {
					// Find the window at the far right
					var x = 300, y = 300;
					$("#main > .window").each(function() {
						var pos = $(this).offset();
						if(pos.left > x){
							x = pos.left;
							y = pos.top;
						}
					});

					newWindow.offset({
						left: x + 200,
						top:  y
					});
				}


				// Draw input connectors
				var xmlInputs  = xmlModule.find("inputs > input");
				// var classInputs = xmlModuleTypes[type].find("inputs > input");
				var offset = 1.0 / (xmlInputs.length + 1);
				var y = offset;

				xmlInputs.each(function(index){
					// Create anchor point for GUI
					var scope = $(this).data('class').find('type').text();
					var color = pointsColor[scope];
					var e1 = jsPlumb.addEndpoint(
						'w' + id, {
							anchor:[0, y, -1, 0], 
							scope: scope,
							paintStyle:{ fillStyle: color},
							connectorStyle:{ strokeStyle: color}
						},
						inputPoint
					);
					$(e1).data('config', $(this));
					$(this).data('gui', $(e1));
					y += offset;
				});

				var xmlOutputs = xmlModule.find("outputs > output");
				offset = 1.0 / (xmlOutputs.length + 1);
				y = offset;
				xmlOutputs.each(function(index){
					// Create anchor point for GUI
					var scope = $(this).data('class').find('type').text();
					var color = pointsColor[scope];
					var e1 = jsPlumb.addEndpoint(
						'w' + id, {
							anchor:[1, y, 1, 0], 
							scope: scope,
							paintStyle:{ fillStyle: color},
							connectorStyle:{ strokeStyle: color}
						},
						outputPoint
					);
					$(e1).data('config', $(this));
					$(this).data('gui', $(e1));
					y += offset;
				});
				
				// Make it draggable
				jsPlumb.draggable(newWindow);
				newWindow.click(function(){
					showDetails(this);	
				});

				// Add events on controls
				newWindow.find(".hide").click(function() {
					jsPlumb.toggleVisible($(this).attr("rel"));
				});
				newWindow.find(".drag").click(function() {
					var s = jsPlumb.toggleDraggable($(this).attr("rel"));
					$(this).html(s ? 'disable dragging' : 'enable dragging');
					if (!s)
						$("#" + $(this).attr("rel")).addClass('drag-locked');
					else
						$("#" + $(this).attr("rel")).removeClass('drag-locked');
					$("#" + $(this).attr("rel")).css("cursor", s ? "pointer" : "default");
				});
				newWindow.find(".detach").click(function() {
					jsPlumb.detachAllConnections($(this).attr("rel"));
				});
				newWindow.find(".delete").click(function() {

					if(!confirm("Do you want to delete this module ?"))
						return;

					jsPlumb.detachAllConnections($(this).attr("rel"));
					var window = $('#' + $(this).attr("rel"));
					jsPlumb.removeAllEndpoints(window);
					window.data('config').remove(); 
					window.remove();
				});
				newWindow.find("#clear").click(function() { 
					jsPlumb.detachEveryConnection();
					showConnectionInfo("");
				});

				newWindow.data('config', xmlModule);
				xmlModule.data('gui', newWindow);

				return newWindow;
			}

			//--------------------------------------------------------------------------------
			// Instanciate a module of a given type
			//--------------------------------------------------------------------------------
			function createNewModule(type){

				var id = maxIdModules;
				var xmlInstance = $("<module/>", xmlProject).appendTo($(xmlProject));
				xmlInstance.attr({
					id: id,
					name: type + id
				});

				// Draw input connectors
				var classInputs    = $(xmlModuleTypes[type]).find("inputs > input");
				var instanceInputs = $('<inputs/>', xmlProject).appendTo(xmlInstance);

				classInputs.each(function(index){
					// Add input to instance
					$('<input/>', xmlProject).appendTo(instanceInputs)
					.attr("id", $(this).attr('id'))
					.data('class', $(this));
				});

				var classOutputs    = $(xmlModuleTypes[type]).find("outputs > output");
				var instanceOutputs = $('<outputs/>', xmlProject).appendTo(xmlInstance);
				classOutputs.each(function(index){
					// Add output to instance
					$('<output/>', xmlProject).appendTo(instanceOutputs)
					.attr('id', $(this).attr('id'))
					.data('class', $(this));
				});
				
				var classParameters  = $(xmlModuleTypes[type]).find("parameters");
				var instanceParameters = $('<parameters/>', xmlProject).appendTo(xmlInstance);
				// note: do not list all parameters: only the class
				classParameters.find('param[name="class"]').each(function(index){
					// Add parameter to instance
					var instance = $('<param/>', xmlProject).appendTo(instanceParameters)
					.attr('name', $(this).attr('name'))
					.data('class', $(this))
					.text($(this).find('value').text());
				});

				
				// Create the associated window
				var newWindow = createModuleWindow(xmlInstance, id);
				xmlInstance.data('class', xmlModuleTypes[type].find('module'));
				maxIdModules++;
			}

			//--------------------------------------------------------------------------------
			// Delete the current modules and load a XML project file 
			//--------------------------------------------------------------------------------
			this.loadProjectFile = function loadProjectFile(fileName) { 
				if(fileName.search(".xml") != fileName.length - 4){
					alert("Project file must be in .xml format");
					return;
				}

				var xml = loadXML(fileName);
				/*if(!xml) 
					xml = loadXML("projects2/" + fileName);*/
				if(!xml) {
					// alert("Error: the file must be inside the 'projects/' or 'projects2/' folder. Sorry for this limitation.");
					alert("Error while loading " + fileName);
					return;
				}

				loadProjectFile2(xml);
			}

			function loadProjectFile2(xml){

				// Load the xml file
				deleteAll();
				xmlProject = $(xml).find("application");

				xmlProject.find('module').each(function(){
					var type = getModuleType($(this));

					$(this).find('inputs > input').each(function(){
						$(this).data('class', xmlModuleTypes[type].find('inputs > input[id="' + $(this).attr('id') + '"]'));
					});
					$(this).find('outputs > output').each(function(){
						$(this).data('class', xmlModuleTypes[type].find('outputs > output[id="' + $(this).attr('id') + '"]'));
					});
					$(this).find('parameters > param').each(function(){
						$(this).data('class', xmlModuleTypes[type].find('parameters > param[name="' + $(this).attr('name') + '"]'));
					});
					$(this).data('class', xmlModuleTypes[type].find('module'));

					// create the window representing the module
					index = parseInt($(this).attr('id'));
					createModuleWindow($(this), index, $(this).find('uiobject'));
					// console.log(index + " " + maxIdModules)
					if(index >= maxIdModules)
						maxIdModules = index + 1;
				});

				// Link inputs-outputs
				xmlProject.find('module').each(function(index){
					$(this).find('inputs > input').each(function(){

						// rebuild connections
						var moduleid = $(this).attr('moduleid');
						var outputid = $(this).attr('outputid');
						if($.isNumeric(moduleid) && $.isNumeric(outputid)) {
							jsPlumb.connect({
								sourceEndpoint: $(this).data('gui')[0],
								targetEndpoint: xmlProject.find('module[id="' + moduleid + '"] > outputs > output[id="' + outputid + '"]').data('gui')[0],
								// type:"basic"
							});
						}
					});
				});
			}
			//--------------------------------------------------------------------------------
			// Display the details of a module in the right panel
			//--------------------------------------------------------------------------------
			function showDetails(window){
				var xml = $(window).data('config');
				$("#explanation").hide();
				var div = $("#detail").show();

				// Module description
				var module = div.find("#module").empty();
				var cl1 = xml.data('class');
				// module.append('<ul>')
				module.append('<li><b>Name: </b>' + cl1.attr('name') + '</li>')
				.append('<li><b>Description: </b>' + cl1.attr('description') + '</li>')
				// .append('</ul>');


				// Show inputs
				var inputs = div.find("#inputs").empty();
				xml.find("inputs > input").each(function(el){
					var cl = $(this).data('class');
					inputs.append('<p>' + cl.find('name').text() + ': ' + cl.find('description').text() + '</p>');
				});
				
				// Show outputs
				var outputs = div.find("#outputs").empty();
				xml.find("outputs > output").each(function(el){
					var cl = $(this).data('class');
					outputs.append('<p>' + cl.find('name').text() + ': ' + cl.find('description').text() + '</p>');
				});
				
				// Show parameters
				var parameters = div.find("#parameters").empty();
				var classParameters  = $(xmlModuleTypes[type]).find("parameters");
				classParameters.find("parameters > param").each(function(el){
					var cl = $(this); // $(this).data('class');
					var value = xml.find("parameters > param[name=" + cl.attr('name') + "]");
					if(value.length <= 0)
						value = '';
					else
						value = ' = ' + value.text();

					parameters.append('<p>' + cl.attr('name') + value + ' (' + cl.find('value').text() + ", " + cl.find('type').text() + ')' + ': ' + cl.find('description').text() + '</p>');
				});
			}


			
			//--------------------------------------------------------------------------------
			// Initialization
			//--------------------------------------------------------------------------------


			if (!_initialised) {

				// Populate select modules with existing modules
				for(var i = 0 ; i < availableModulesNames.length ; i++) {
					var type = availableModulesNames[i];

					// Add the module for module creation
					$("#selectModule").append('<option value=' + availableModulesNames[i] + '>' + availableModulesNames[i] + '</option>');

					// Load the matching xml file
					xmlModuleTypes[type] = $(loadXML("modules/" + availableModulesNames[i] + ".xml")).find("module");
				}
				
				// Populate select projects with existing projects
				for(var i = 0 ; i < availableProjectsNames.length ; i++) {
					$("#selectProjectFile").append('<option value=' + availableProjectsNames[i] + '>' + availableProjectsNames[i] + '</option>');
				}
				
				// Create a div representing a new module
				$("#createModule").click(function() {
					var type = $("#selectModule").val();
					createNewModule(type);
				});
				// Create a div representing a new module
				$("#downloadProject").click(function() {

					// Include position information in the xml
    					xmlProject.find('module').each(function(){
						var window = $(this).data('gui');
						var pos = window.offset();
						$(this).find('uiobject').remove();
						$('<uiobject/>', xmlProject).appendTo($(this))
						.attr({
							// type:   getModuleType($(this)),
							x:      pos.left,
							y:      pos.top,
							width:  "0",
							height: "0"
						});
					});

					if (! window['XSLTProcessor'])
					{
						// Trasformation for IE
						// Note: not tested
						transformed = xml.transformNode(xmlProject);
					}
					else
					{
						// Transformation for non-IE
						// var processor = new XSLTProcessor();
						// processor.importStylesheet(xslt);
						// var xmldom = processor.transformToDocument(xml);
						var serializer = new XMLSerializer();
						var transformed = serializer.serializeToString(xmlProject[0]);
					}
					transformed = transformed.replace(/>[ ]*</g, '><').replace(/></g, '>\r\n<');
					var win = window.open('data:text/xml,<?xml version="1.0" encoding="UTF-8"?>' + encodeURIComponent(transformed), 'Project', '', true)
				});
				$("#loadProjectFile").click(function (){
					window.markusEditor.loadProjectFile($("#selectProjectFile").val());
				});
				$("#deleteAll").click(function() {
					if(confirm("Do you want to delete all existing modules ?"))
						deleteAll();
				});
				$('#dropZone').on('dragover', function(e) {
					e.preventDefault();
					e.stopPropagation();
				});
				$('#dropZone').on('dragenter', function(e) {
					e.preventDefault();
					e.stopPropagation();
				});
				$('#dropZone').on('drop', function(e) {
					// Read a project file from the drag and drop div
					var reader = new FileReader();
					if(e.originalEvent.dataTransfer){
						if(e.originalEvent.dataTransfer.files.length) {
							e.preventDefault();
							e.stopPropagation();
							var file = e.originalEvent.dataTransfer.files[0];
							if(file.type != 'text/xml') {
								alert('File must be of type XML');
								return;
							}
							reader.onload = function(e) {
								var text = reader.result;
								var xml = parseXml(text);
								loadProjectFile2(xml);
							}

							reader.readAsText(file, "UTF-8");
						}   
					}
				});
					
					
				deleteAll();
				_initialised = true;
			}
		}
	};	
})();
/*
 *  This file contains the JS that handles the first init of each jQuery demonstration, and also switching
 *  between render modes.
 */
jsPlumb.bind("ready", function() {
	// render mode
	var resetRenderMode = function(desiredMode) {
		var newMode = jsPlumb.setRenderMode(desiredMode);
		$(".rmode").removeClass("selected");
		$(".rmode[mode='" + newMode + "']").addClass("selected");

		$(".rmode[mode='canvas']").attr("disabled", !jsPlumb.isCanvasAvailable());
		$(".rmode[mode='svg']").attr("disabled", !jsPlumb.isSVGAvailable());
		$(".rmode[mode='vml']").attr("disabled", !jsPlumb.isVMLAvailable());
		     
		markusEditor.init();
	};

	$(".rmode").bind("click", function() {
		var desiredMode = $(this).attr("mode");
		if (markusEditor.reset) markusEditor.reset();
		jsPlumb.reset();
		resetRenderMode(desiredMode);
	});

	resetRenderMode(jsPlumb.SVG);
});
