import {
	Component,
	Input,
	Output,
	ViewEncapsulation,
	AfterViewInit,
	AfterViewChecked,
	EventEmitter
} from '@angular/core';
import {Project}  from './project';
import {Module}  from './module';
import {Utils} from './utils';


// preemptive declaration for external js object
declare var jsPlumb: any;

@Component({
	selector: 'project-window',
	template: `
<top-bar
	[project]='project'
	(createModule)='onCreate($event)'
	(uploadProject)='onUploadProject($event)'
	(deleteAll)='onDeleteAll(true)'
></top-bar>
<div class='jtk-demo-canvas canvas-wide drag-drop-demo jtk-surface jtk-surface-nopan' id='canvas'>
	<module 
		*ngFor='let mod of project.modules'
		[module]='mod' 
		[moduleDescription]='findClassDescription(mod.class)'
		[jsPlumbInstance]='jsPlumbInstance'
		(deleteModule)='onDelete($event)'
		(selectModule)='onSelect($event)'
	></module>
</div>

`,
	styles: [
`.project-window {
	margin-left: 25%;
	height: 300px;
	background: red;
}`
	],
	encapsulation: ViewEncapsulation.Emulated
})
export class ProjectWindowComponent implements AfterViewInit, AfterViewChecked {
	@Input()  private project: Project;
	@Input()  private hasChanges: boolean;
	@Input()  private moduleDescriptions: Array<Module>;
	@Output() private selectModule: EventEmitter<string> = new EventEmitter<string>();
	private jsPlumbInstance: any;
	private reconnect: boolean = false;

	findClassDescription(className: string): any {
		return Utils.findByClass(this.moduleDescriptions, className);
	}

	onSelect(event: string): void {
		this.selectModule.emit(event);
	}

	onDelete(event: string): void {
		Utils.removeByName(this.project.modules, event);
		this.hasChanges = true;
		this.onSelect(undefined); // as a safety
	}

	onCreate(event: string): void {
		let i = 0;
		while(Utils.findByName(this.project.modules, event + i) !== undefined) {
			i++;
		}
		let x = 0, y = 0;
		for(let mod of this.project.modules) {
			if(mod.uiobject.x > x) {
				x = mod.uiobject.x;
				y = mod.uiobject.y;
			}
		}
		this.project.modules.push({name: event + i, class: event, inputs:[], outputs:[], uiobject:{x: x + 200, y: y}});
		this.hasChanges = true;
	}
	onDeleteAll(ask: boolean): void {
		if(ask && !confirm('Delete all modules ?'))
			return;
		// this.jsPlumbInstance.detachEveryConnection();
		this.project.modules = [];
		this.jsPlumbInstance.deleteEveryEndpoint();
		this.hasChanges = true;
		this.onSelect(undefined);
	}
	onUploadProject(event: any): void {

		if(event.srcElement.files.length !== 1)
			alert('You need to select one file: ' + event.srcElement.files.length + ' selected');
		let file = event.srcElement.files[0];
		let reader = new FileReader();
		reader.readAsText(file, 'UTF-8');
		reader.onload = (evt: any) => {
			// note: needed for a clean delete
			this.onDeleteAll(false);

			Project.readFromFile(this.project, evt.target.result);
			this.reconnect = true;
			this.hasChanges = false;
		};
		reader.onerror = function (evt: any) {
			alert('Error reading JSON file');
		};
	}
	public ngAfterViewChecked(): void {
		// note: we have to use a reconnect state variable as:
		//       - the view must have been initialized
		//       - reconnection must be done once only
		if(!this.reconnect)
			return;
		this.reconnect = false;
		// connect inputs if any
		for(let mod of this.project.modules) {
			for(let inp of mod.inputs) {
				if(inp.connected) {
					this.jsPlumbInstance.connect({
						uuids:[
							inp.connected.module + '_o_' + inp.connected.output,
							mod.name + '_i_' + inp.name
						]
					});
				}
			}
		}
	}

	// Connect input and output
	connectInput(params: any) {
		let inputModule = Utils.findByName(this.project.modules, params.inputModule);
		let inputStream = Utils.findByNameAndAdd(inputModule.inputs, params.inputStream);
		inputStream.connected = {
			module: params.outputModule,
			output: params.outputStream
		};
		this.hasChanges = true;
	}

	// Disconnect input and output
	disconnectInput(params: any) {
		let inputModule = Utils.findByName(this.project.modules, params.inputModule);
		let inputStream = Utils.findByNameAndAdd(inputModule.inputs, params.inputStream);
		inputStream.connected = undefined;
		this.hasChanges = true;
	}

	/*
	 *  This file contains the JS that handles the first init of each jQuery demonstration, and also switching
	 *  between render modes.
	 */
	ngAfterViewInit(): void {
		jsPlumb.ready(() => {

			jsPlumb.importDefaults({
				DragOptions: { cursor: 'pointer', zIndex:2000 },
				PaintStyle: { stroke:'#666' },
				EndpointStyle: { width:20, height:16, stroke:'#666' },
				Endpoint: 'Rectangle',
				Anchors: ['TopCenter', 'TopCenter']
			});
			this.jsPlumbInstance = jsPlumb.getInstance({
				DragOptions: { cursor: 'pointer', zIndex:2000 },
				PaintStyle: { stroke:'#666' },
				EndpointHoverStyle: { fill: 'orange' },
				HoverPaintStyle: { stroke: 'orange' },
				EndpointStyle: { width:20, height:16, stroke:'#666' },
				Endpoint: 'Rectangle',
				Anchors: ['TopCenter', 'TopCenter'],
				Container: 'canvas'
			});

			// Events linked to connection of inputs and outputs
			this.jsPlumbInstance.bind('connection', function(params: any, originalEvent: any) {
				// Check if this was done by the user
				if(originalEvent === undefined)
					return;
				this.connectInput({
					inputModule:  params.targetId,
					inputStream:  params.targetEndpoint.getParameter('name'),
					outputModule: params.sourceId,
					outputStream: params.sourceEndpoint.getParameter('name')
				});

				return true;
			});
			this.jsPlumbInstance.bind('beforeDetach', function(params: any, originalEvent: any) {
				// Check if this was done by the user
				if(originalEvent === undefined)
					return;
				this.disconnectInput({
					inputModule:  params.targetId,
					inputStream:  params.suspendedEndpoint.getParameter('name'),
					outputModule: params.sourceId,
					outputStream: params.endpoints[0].getParameters()
				});

				return true;
			});
		});
	}
}
