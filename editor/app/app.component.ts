import {Component, NgZone, OnDestroy} from '@angular/core';
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
		[reconnect]='reconnect'
		[moduleDescriptions]='moduleDescriptions' 
		(selectModule)='onSelect($event)'
		(loadProjectJson)=onLoadProjectJson($event)
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
	public project: Project = {name:'emptyProject', description:'', modules:[]};
	private selectedModule: Module;
	private selectedModuleDescription: Module;
	public hasChanges: boolean = false;
	public reconnect: boolean = false;
	private moduleDescriptions: any = {all: []};
	constructor(private http: Http, private ngZone: NgZone) {
		http.get('modules/moduleDescriptions.json').map((res: Response) => res.json()).subscribe(res => this.moduleDescriptions = res);

		window.markusEditor = window.markusEditor || {
			loadProjectJson: (json: string) => this.publicLoadProjectJson(json),
			dumpProject: () => this.dumpProject(),
			hasChanged: () => this.hasChanged()
		};

		// For control from our Qt editor
		if(typeof g_controller !== 'undefined')
			g_controller.app = this;
	}

	publicLoadProjectJson(json: string) {
		this.ngZone.run(() => {
			this.project = JSON.parse(json);
			this.reconnect = true;
			this.hasChanges = false;
		});
	}

	dumpProject():  string {
		return JSON.stringify(this.project);
	}

	hasChanged():  boolean {
		return this.hasChanges;
	}

	onSelect(event: string): void {
		this.selectedModule = Utils.findByName(this.project.modules, event);
		this.selectedModuleDescription = this.selectedModule ? Utils.findByClass(this.moduleDescriptions, this.selectedModule.class) : undefined;
	}

	ngOnDestroy() {
		window.markusEditor = null;
	}
}
