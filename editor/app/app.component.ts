import {Component} from '@angular/core';
import {Module} from './module';
import {Project} from './project';
import {Http, Response} from '@angular/http';
import {Utils} from './utils';

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
</div>
`
})
export class AppComponent {
	private project: Project = {name:'emptyProject', description:'', modules:[]};
	private selectedModule: Module;
	private selectedModuleDescription: Module;
	private hasChanges: boolean = false;
	private moduleDescriptions: any = {all: []};
	constructor(http: Http) {
		http.get('modules/moduleDescriptions.json').map((res: Response) => res.json()).subscribe(res => this.moduleDescriptions = res);
	}
	onSelect(event: string): void {
		this.selectedModule = Utils.findByName(this.project.modules, event);
		this.selectedModuleDescription = this.selectedModule ? Utils.findByClass(this.moduleDescriptions, this.selectedModule.class) : undefined;
	}
}
