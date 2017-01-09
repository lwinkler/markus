import {
	Component,
	Input,
	Output,
	ViewEncapsulation,
	AfterViewInit,
	EventEmitter
} from '@angular/core';
import {Module}  from './module';

let colorDefault = '#316b31';

// Configuration of endpoints for jsPlumb
function getPointColor(type: string) {

	switch(type) {
		case 'Image':      return '#316b31';
		case 'Objects':    return '#00f';
		case 'State':      return '#cd7f32';
		case 'Event':      return '#df0101';
		case 'NumBool':    return '#09098e';
		case 'NumDouble':  return '#09098e';
		case 'NumInt':     return '#09098e';
		case 'NumUInt':    return '#09098e';
		case 'NumFloat':   return '#09098e';
		default:
			console.log('Warning: No color for stream of type ' + type);
		return '#316b31';
	}
}

// configure some drop options for use by all endpoints.
let dropOptions = {
	tolerance:'touch',
	hoverClass:'dropHover',
	activeClass:'dragActive'
};

let inputPoint = {
	endpoint:'Rectangle',
	paintStyle: { fill: colorDefault },
	isSource:false,
	scope:'default',
	connectorStyle:{ stroke:colorDefault, lineWidth:2 },
	maxConnections:1,
	isTarget:true,
	dropOptions: dropOptions
};

let inputPointMulti = {
	endpoint:'Rectangle',
	paintStyle:{ fill:colorDefault },
	isSource:false,
	scope:'default',
	connectorStyle:{ stroke:colorDefault, lineWidth:2 },
	maxConnections:99,
	isTarget:true,
	dropOptions: dropOptions
};

let outputPoint = {
	endpoint:['Dot', { radius:10 }],
	paintStyle:{ fill:colorDefault },
	isSource:true,
	scope:'default',
	connectorStyle:{ stroke:colorDefault, lineWidth:2 },
	connector: ['Bezier', { curviness:63 } ],
	maxConnections:99,
	isTarget:false,
	dropOptions: dropOptions
};


@Component({
	selector: 'module',
	host: {
		'[id]': 'module.name',
		'class': 'window',
		'(mousedown)': 'onSelect($event)',
		'(mousemove)': 'onMouseLeave($event)',
		'[style.left.px]': 'module.uiobject.x',
		'[style.top.px]': 'module.uiobject.y'
	},
	template: `
<h3>{{module.name}}</h3>
<p>{{module.class}}</p>
<p><a class='cmdLink delete' rel='{{module.name}}' (click)='onDelete($event)'>delete</a></p>
`,
	styles: [
`
module {
	z-index: 3;
}
`
	],
	encapsulation: ViewEncapsulation.Emulated
})
export class ModuleComponent implements AfterViewInit {
	@Input()        module: Module;
	@Input()        moduleDescription: any;
	@Input()        jsPlumbInstance: any;
	@Output()       selectModule: EventEmitter<string> = new EventEmitter<string>();
	@Output()       deleteModule: EventEmitter<string> = new EventEmitter<string>();

	onSelect(): void {
		this.selectModule.emit(this.module.name);
	}
	onMouseLeave(event: any): boolean {
		let el = event.srcElement;
		while(el.tagName != "MODULE")
			el = el.parentElement;
		// As a small trick we update the position here
		// maybe could be done cleaner but how ?
		this.module.uiobject = {
			x: el.offsetLeft,
			y: el.offsetTop
		};
		return true;
	}
	onDelete(): void {
		if(!confirm('Do you want to delete this module ?'))
			return;

		this.deleteModule.emit(this.module.name);
	}
	ngOnDestroy(): void {
		this.jsPlumbInstance.detachAllConnections(this.module.name);
		this.jsPlumbInstance.removeAllEndpoints(this.module.name);
	}
	ngAfterViewInit(): void {

		// Draw input connectors
		function countStreams(inputs: Array<any>): number {
			let cpt = 0;
			for(let input of inputs) {
				if(input.stream)
					cpt++;
			}
			return cpt;
		}
		let offset = 1.0 / (countStreams(this.moduleDescription.inputs) + 1);
		let y = offset;

		for(let input of this.moduleDescription.inputs) {
			// Create anchor point for GUI
			if(!input.stream)
				continue;
			let color = getPointColor(input.type);
			if(input.multi === undefined) {
				// note: we need labels to store the input name !!
				this.jsPlumbInstance.addEndpoint(
					this.module.name, {
						anchor:[0, y, -1, 0],
						scope: input.type,
						paintStyle:{ fill: color},
						connectorStyle:{stroke: color},
						parameters: {
							name: input.name
						},
						uuid: this.module.name + '_' + input.name
					},
					inputPoint
				);
			} else {
				this.jsPlumbInstance.addEndpoint(
					this.module.name, {
						anchor:[0, y, -1, 0],
						scope: input.type,
						paintStyle:{ fill: color, lineWidth: 5},
						connectorStyle:{stroke: color},
						parameters: {
							name: input.name
						},
						uuid: this.module.name + '_' + input.name
					},
					inputPointMulti
				);
			}
			y += offset;
		};

		offset = 1.0 / (countStreams(this.moduleDescription.outputs) + 1);
		y = offset;
		for(let output of this.moduleDescription.outputs) {
			// Create anchor point for GUI
			let color = getPointColor(output.type);
			this.jsPlumbInstance.addEndpoint(
				this.module.name,
				{
					anchor:[1, y, 1, 0],
					scope: output.type,
					paintStyle:{ fill: color},
					connectorStyle:{ stroke: color},
					parameters: {
						name: output.name
					},
					uuid: this.module.name + '_' + output.name
				},
				outputPoint
			);
			y += offset;
		};

		// Make it draggable
		this.jsPlumbInstance.draggable(this.module.name, {
			grid:[50, 50]
		});
	}
}
