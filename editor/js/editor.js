// Global objects
var availableModulesNames = [
	"BackgroundExtractor", 
	"BlobSegmenter",
	"CascadeDetector",
	"SimpleTracker",
	"SlitCamera",
	"TemporalDifferencing",
	"UsbCam",
	"VideoFileReader",
	"VideoOutput"
];

var nbModules = 0;
var xmlModuleTypes = [];
var xmlProject = "";


;(function() {


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
	
	window.jsPlumbDemo = {
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

			//
			// first example endpoint.  it's a 25x21 rectangle (the size is provided in the 'style' arg to the Endpoint),
			// and it's both a source and target.  the 'scope' of this Endpoint is 'exampleConnection', meaning any connection
			// starting from this Endpoint is of type 'exampleConnection' and can only be dropped on an Endpoint target
			// that declares 'exampleEndpoint' as its drop scope, and also that
			// only 'exampleConnection' types can be dropped here.
			//
			// the connection style for this endpoint is a Bezier curve (we didn't provide one, so we use the default), with a lineWidth of
			// 5 pixels, and a gradient.
			//
			// there is a 'beforeDrop' interceptor on this endpoint which is used to allow the user to decide whether
			// or not to allow a particular connection to be established.
			//
			/*var exampleColor = "#00f";
			var exampleEndpoint = {
				endpoint:"Rectangle",
				paintStyle:{ width:25, height:21, fillStyle:exampleColor },
				isSource:true,
				reattach:true,
				scope:"blue rectangle",
				connectorStyle : {
					gradient:{stops:[[0, exampleColor], [0.5, "#09098e"], [1, exampleColor]]},
					lineWidth:5,
					strokeStyle:exampleColor,
					dashstyle:"2 2"
				},
				isTarget:true,
				beforeDrop:function(params) { 
					return confirm("Connect " + params.sourceId + " to " + params.targetId + "?"); 
				},				
				dropOptions : dropOptions
			};*/			

			//
			// the second example uses a Dot of radius 15 as the endpoint marker, is both a source and target,
			// and has scope 'exampleConnection2'.
			//
			var colorDefault = "#316b31";
			
			var pointsColor = {
				Image: "#316b31",	
				Objects: "#00f",	
				Debug: "#09098e"
			};


			var inputPoint = {
				endpoint:"Rectangle",
				paintStyle:{ fillStyle:colorDefault },
				isSource:false,
				scope:"default",
				connectorStyle:{ strokeStyle:colorDefault, lineWidth:2 },
				// connector: ["Bezier", { curviness:63 } ],
				maxConnections:1,
				isTarget:true,
				beforeDrop:function(params) { 
					// Connect input with output on xml

					var xmlOutput = $(params.connection.endpoints[0]).data('config');
					var xmlInput  = $(params.dropEndpoint).data('config');

					// if(!! xmlInput && !! xmlOutput)
					// return false;
					xmlInput.attr('outputid', xmlOutput.attr('id'));
					xmlInput.attr('moduleid', xmlOutput.parent().parent().attr('id'));

					return true;

				},				
				dropOptions : dropOptions
			};


			// inputPoint.bind("maxConnections", maxConnectionsCallback);

			var outputPoint = {
				endpoint:["Dot", { radius:10 }],
				paintStyle:{ fillStyle:colorDefault },
				isSource:true,
				scope:"default",
				connectorStyle:{ strokeStyle:colorDefault, lineWidth:2 },
				// connector: ["Bezier", { curviness:63 } ],
				maxConnections:99,
				isTarget:false,
				dropOptions : dropOptions
			};
			//
			// the third example uses a Dot of radius 17 as the endpoint marker, is both a source and target, and has scope
			// 'exampleConnection3'.  it uses a Straight connector, and the Anchor is created here (bottom left corner) and never
			// overriden, so it appears in the same place on every element.
			//
			// this example also demonstrates the beforeDetach interceptor, which allows you to intercept 
			// a connection detach and decide whether or not you wish to allow it to proceed.
			//			
			/*var example3Color = "rgba(229,219,61,0.5)";
			var exampleEndpoint3 = {
				endpoint:["Dot", {radius:17} ],
				anchor:"BottomLeft",
				paintStyle:{ fillStyle:example3Color, opacity:0.5 },
				isSource:true,
				scope:'yellow dot',
				connectorStyle:{ strokeStyle:example3Color, lineWidth:4 },
				connector : "Straight",
				isTarget:true,
				dropOptions : dropOptions,
				beforeDetach:function(conn) { 
					return confirm("Detach connection?"); 
				},
				onMaxConnections:function(info) {
					alert("Cannot drop connection " + info.connection.id + " : maxConnections has been reached on Endpoint " + info.endpoint.id);
				}
			};*/
/*
			// setup some empty endpoints.  again note the use of the three-arg method to reuse all the parameters except the location
			// of the anchor (purely because we want to move the anchor around here; you could set it one time and forget about it though.)
			var e1 = jsPlumb.addEndpoint('window1', { anchor:[0.5, 1, 0, 1] }, exampleEndpoint2);

			// setup some DynamicAnchors for use with the blue endpoints			
			// and a function to set as the maxConnections callback.
			var anchors = [[1, 0.2, 1, 0], [0.8, 1, 0, 1], [0, 0.8, -1, 0], [0.2, 0, 0, -1] ],
				maxConnectionsCallback = function(info) {
					alert("Cannot drop connection " + info.connection.id + " : maxConnections has been reached on Endpoint " + info.endpoint.id);
				};
				
			var e1 = jsPlumb.addEndpoint("window1", { anchor:anchors }, exampleEndpoint);
			// you can bind for a maxConnections callback using a standard bind call, but you can also supply 'onMaxConnections' in an Endpoint definition - see exampleEndpoint3 above.
			e1.bind("maxConnections", maxConnectionsCallback);

			var e2 = jsPlumb.addEndpoint('window2', { anchor:[0.5, 1, 0, 1] }, exampleEndpoint);
			// again we bind manually. it's starting to get tedious.  but now that i've done one of the blue endpoints this way, i have to do them all...
			e2.bind("maxConnections", maxConnectionsCallback);
			jsPlumb.addEndpoint('window2', { anchor:"RightMiddle" }, exampleEndpoint2);

			var e3 = jsPlumb.addEndpoint("window3", { anchor:[0.25, 0, 0, -1] }, exampleEndpoint);
			e3.bind("maxConnections", maxConnectionsCallback);
			jsPlumb.addEndpoint("window3", { anchor:[0.75, 0, 0, -1] }, exampleEndpoint2);

			var e4 = jsPlumb.addEndpoint("window4", { anchor:[1, 0.5, 1, 0] }, exampleEndpoint);
			e4.bind("maxConnections", maxConnectionsCallback);			
			jsPlumb.addEndpoint("window4", { anchor:[0.25, 0, 0, -1] }, exampleEndpoint2);

			// make .window divs draggable
			jsPlumb.draggable($(".window"));

			// add endpoint of type 3 using a selector. 
			jsPlumb.addEndpoint($(".window"), exampleEndpoint3);
*/
			//-------------------------------------------------------------------------------- 
			// Functions and utilities
			//--------------------------------------------------------------------------------

			/* Load an xml file */
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

			function createModuleWindow(type, id) {
				var newWindow = $('<div/>', {
					class:"window",
					id: id
				})
				.append('<h3 id="name">' + type + " 1" + '</h3>')
				.append('<p id="type">' + type + '</p>')
				.append('<a href="#" class="cmdLink hide"   rel="' + id + '">toggle connections</a><br/>')
				.append('<a href="#" class="cmdLink drag"   rel="' + id + '">disable dragging</a><br/>')
				.append('<a href="#" class="cmdLink detach" rel="' + id + '">detach all</a>');

				// Append the module window to main div 
				$("#main").append(newWindow);

				return newWindow;
			}

			/* Instanciate a module of a known type */

			function createNewModule(type){
				var id = /*type +*/ nbModules;
				nbModules++;

				newWindow = createModuleWindow(type, 'w' + id);

				// var xml = $(xmlProject).find("application").append($(xmlModuleTypes[type]).find("module").clone());
				var xml = $(xmlModuleTypes[type]).find("module").clone().appendTo($(xmlProject).find("application"));
				xml.attr('id', id);

				// Draw input connectors
				var inputs = xml.find(" inputs > input");
				var offset = 1.0 / (inputs.length + 1);
				var y = offset;
				inputs.each(function(el){
					var scope = $(this).find('type').text();
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
					y += offset;
				});

				// Draw output connectors
				var outputs = xml.find(" outputs > output");
				offset = 1.0 / (outputs.length + 1);
				y = offset;
				outputs.each(function(){
					var scope = $(this).find('type').text();
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
					// Add a link to the xml config
					$(e1).data('config', $(this));
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
				newWindow.find("#clear").click(function() { 
					jsPlumb.detachEveryConnection();
					showConnectionInfo("");
				});
				newWindow.data('config', xml);
			}

			/* display the detail of the module in the right panel */
			function showDetails(window){
				var xml = window.data('config');
				$("#explanation").hide();
				var div = $("#detail").show();
				
				// Show inputs
				var inputs = div.find("#inputs").empty();
				xml.find("inputs > input").each(function(el){
					inputs.append('<p>' + $(this).find('name').text() + ': ' + $(this).find('description').text() + '</p>');
				});
				
				// Show outputs
				var outputs = div.find("#outputs").empty();
				xml.find("outputs > output").each(function(el){
					outputs.append('<p>' + $(this).find('name').text() + ': ' + $(this).find('description').text() + '</p>');
				});
				
				// Show inputs
				var parameters = div.find("#parameters").empty();
				xml.find("parameters > param").each(function(el){
					parameters.append('<p>' + $(this).find('value').text() + '(' + $(this).find('type').text() + ')' + ': ' + $(this).find('description').text() + '</p>');
				});
				

			}


			
			//--------------------------------------------------------------------------------
			// Initialization
			//--------------------------------------------------------------------------------


			if (!_initialised) {
				/*$(".hide").click(function() {
					jsPlumb.toggleVisible($(this).attr("rel"));
				});
	
				$(".drag").click(function() {
					var s = jsPlumb.toggleDraggable($(this).attr("rel"));
					$(this).html(s ? 'disable dragging' : 'enable dragging');
					if (!s)
						$("#" + $(this).attr("rel")).addClass('drag-locked');
					else
						$("#" + $(this).attr("rel")).removeClass('drag-locked');
					$("#" + $(this).attr("rel")).css("cursor", s ? "pointer" : "default");
				});
	
				$(".detach").click(function() {
					jsPlumb.detachAllConnections($(this).attr("rel"));
				});
	
				$("#clear").click(function() { 
					jsPlumb.detachEveryConnection();
					showConnectionInfo("");
				});*/

				// Populate select modules with existing modules

				for(var i = 0 ; i < availableModulesNames.length ; i++) {
					var type = availableModulesNames[i];

					// Add the module for module creation
					$("#selectModule").append('<option value=' + availableModulesNames[i] + '>' + availableModulesNames[i] + '</option>');

					// Load the matching xml file
					xmlModuleTypes[type] = loadXML("modules/" + availableModulesNames[i] + ".xml");
				}
				
				// Create a div representing a new module
				$("#createModule").click(function() {
					var type = $("#selectModule").val();
					createNewModule(type);
				});
				// Create a div representing a new module
				$("#downloadProject").click(function() {
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
						var transformed = serializer.serializeToString(xmlProject.documentElement);
					}

					var win = window.open('data:text/xml,<?xml version="1.0" encoding="UTF-8"?>' + transformed, 'Project', '', true)
				});

				// Load an empty project
				xmlProject = loadXML("editor/xml/EmptyProject.xml");
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

	/*jsPlumb.DemoList.init(); */

	// render mode
	var resetRenderMode = function(desiredMode) {
		var newMode = jsPlumb.setRenderMode(desiredMode);
		$(".rmode").removeClass("selected");
		$(".rmode[mode='" + newMode + "']").addClass("selected");

		$(".rmode[mode='canvas']").attr("disabled", !jsPlumb.isCanvasAvailable());
		$(".rmode[mode='svg']").attr("disabled", !jsPlumb.isSVGAvailable());
		$(".rmode[mode='vml']").attr("disabled", !jsPlumb.isVMLAvailable());
		     
		jsPlumbDemo.init();
	};

	$(".rmode").bind("click", function() {
		var desiredMode = $(this).attr("mode");
		if (jsPlumbDemo.reset) jsPlumbDemo.reset();
		jsPlumb.reset();
		resetRenderMode(desiredMode);
	});

	resetRenderMode(jsPlumb.SVG);

});
