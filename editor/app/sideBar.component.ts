import { Component, Input } from '@angular/core';
import {ViewEncapsulation}  from '@angular/core';
import {Module} from './module';
import {Utils} from './utils';

@Component({
	selector: 'side-bar',
	template: `
<div class='side-bar'>
	<div *ngIf='module===undefined'>
		<h3>Markus module editor</h3>
			<p>This is the visual module editor for Markus. With this simple web interface you can open, visualize and edit projects. </p>
			<ul>
				<li><b>Create a new module: </b>Add the module you need using the 'Create' selection box above and connect the modules'inputs and outputs by drag and drop.</li>
				<li><b>Download the project: </b>This opens a new tab to your browser. You can save it as an xml file under the 'project/' directory.</li>
				<li><b>Clean: </b>Remove all modules from the board and start a new project.</li>
				<li><b>Load a project: </b>Load an existing project</li>
			</ul>
		<h3>Tips</h3>
			<ul>
				<li><b>Supported browsers: </b>So far the module editor has only been tested with Firefox. It should work with Chrome as well but only through a web server. (E.g. Apache)</li>
				<li><b>XML files: </b>Modules classes (with inputs and outputs) are defined in xml files inside the 'module/' directory. Project files are saved as XML files inside the project directory.</li>
			</ul>
	</div>
	<div *ngIf='module!==undefined'>
		<h3>{{module.name}} ({{module.class}}) [{{module.uiobject.x}}, {{module.uiobject.y}}]</h3>
		<!-- Causes problems with jsPlumb input type='text' [(ngModel)]='module.name'/-->
		<h3>Inputs</h3>
		<table class='stream-table'>
			<tr><th>Name</th><th>Type</th><th>Default</th><th>Range</th><th>Set</th><th>Value</th></tr>
			<tr *ngFor='let stream of moduleDescription.inputs'>
				<td>{{stream.name}}</td>
				<td>{{stream.type}}</td>
				<td><json-modal [object]='stream.default'></json-modal></td>
				<td><json-modal [object]='stream.range'></json-modal></td>
				<td><input-set [description]='stream' [inputs]='module.inputs' [input]='findInput(stream.name)'></input-set></td>
				<td style='display: inline-block; width: 100px'><input-value [description]='stream' [input]='findInput(stream.name)'></input-value></td>
			</tr>
		</table>
		<h3>Outputs</h3>
		<table class='stream-table'>
			<tr><th>Name</th><th>Type</th><th>Default</th><th>Range</th></tr>
			<tr *ngFor='let stream of moduleDescription.outputs'>
				<td>{{stream.name}}</td>
				<td><input value='{{stream.type}}'/></td>
				<td><json-modal [object]='stream.default'></json-modal></td>
				<td><json-modal [object]='stream.range'></json-modal></td>
			</tr>
		</table>
	</div>
</div>
`
	,
	styles: [
`.side-bar {
	width: 25%;
	float: left;
}
h3 {
	white-space:nowrap
}
`
	],
	encapsulation: ViewEncapsulation.Emulated
})
export class SideBarComponent {
	@Input()
	module: Module;
	@Input()
	moduleDescription: any;
	@Input()
	hasChanges: boolean; // TODO

	findInput(name: string) {
		return Utils.findByName(this.module.inputs, name);
	}
	// findOutput(name: string) {
		// return Utils.findByName(this.module.outputs, name);
	// }
}

