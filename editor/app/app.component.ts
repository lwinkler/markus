import {Component} from '@angular/core';
import {Module} from './module';
import {Project} from './project';
import {Http, Response} from '@angular/http';
import {Utils} from './utils';

declare var g_controller: any;

@Component({
	selector: 'markus-editor',
	template: `
<div class='markus-editor'>
	<project-window 
		[project]='project'
		[hasChanges]='hasChanges'
		[moduleDescriptions]='moduleDescriptions' 
		(selectModule)='onSelect($event)'
	></project-window>
	<side-bar 
		[module]='selectedModule'
		[hasChanges]='hasChanges'
		[moduleDescription]='selectedModuleDescription'
	></side-bar>

	<db-object-viewer></db-object-viewer>
</div>
`
})
export class AppComponent {
	public project: Project = {name:'emptyProject', description:'', modules:[]};
	private selectedModule: Module;
	private selectedModuleDescription: Module;
	public hasChanges: boolean = false;
	private moduleDescriptions: any = {all: []};
	constructor(http: Http) {
		http.get('modules/moduleDescriptions.json').map((res: Response) => res.json()).subscribe(res => this.moduleDescriptions = res);

		// For control from our Qt editor
		if(typeof g_controller !== 'undefined')
			g_controller.app = this;
	}
	onSelect(event: string): void {
		this.selectedModule = Utils.findByName(this.project.modules, event);
		this.selectedModuleDescription = this.selectedModule ? Utils.findByClass(this.moduleDescriptions, this.selectedModule.class) : undefined;
	}
}
