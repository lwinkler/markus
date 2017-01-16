import {Component} from '@angular/core';
import {Http, Response} from '@angular/http';
import {Module} from './module';
import {Project} from './project';
import {Utils} from './utils';
import {Routes /*,  ROUTER_DIRECTIVES, ROUTER_PROVIDERS, LocationStrategy, HashLocationStrategy*/} from '@angular/router'

declare var g_controller: any;

@Component({
	selector: 'markus-editor',
	template: `
<div class='markus-app'>
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
</div>
`
})
export class MarkusEditor {
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
