import {
	Component,
	Input,
	Output,
	EventEmitter,
	ViewEncapsulation,
	Injectable
} from '@angular/core';
import {Http, Response} from '@angular/http'
import {Project}  from './project';
import {Utils} from './utils';
import 'rxjs/add/operator/map'

@Component({
	selector: 'top-bar',
	template: `
<div class='top-bar'>
	Project name <input type='text' [(ngModel)]='project.name' placeholder='projectFile'/>
	<button (click)='onDownload($event)'>Download</button>
	<button (click)='onDeleteAll($event)'>Delete all</button>
	<a id='downloadAnchorElem' style='display:none'>Download link</a>
	<label>Load:</label> <input type='file' accept='.json' (change)='onUploadProject($event)'/>
	<select [(ngModel)]='categorieToAdd' (change)='onChange($event)'>
		<option *ngFor='let cat of moduleCategories' [value]='cat.name'>{{cat.name}}</option>
	</select>
	<select [(ngModel)]='moduleToAdd'>
		<option *ngFor='let mod of moduleCategorie.modules' [value]='mod'>{{mod}}</option>
	</select>
	<button (click)='onCreate($event)'>Add module</button>
</div>
`,
	styles: [
`top-bar {
	width: 100%;
	float: top;
}`
	],
	encapsulation: ViewEncapsulation.Emulated
})
@Injectable()
export class TopBarComponent {
	@Input()
	project: Project = undefined;
	categorieToAdd : string = 'all';
	moduleToAdd : string;
	moduleCategories: Array<Object> = [{name:'all', modules: []}];
	moduleCategorie: any = {name: 'all', modules:[]};
	constructor(http : Http) {
		http.get('./moduleCategories.json').map((res: Response) => res.json()).subscribe(res => this.moduleCategories = res);
	}
	@Output() createModule:  EventEmitter<string> = new EventEmitter<string>();
	@Output() uploadProject: EventEmitter<string> = new EventEmitter<string>();
	@Output() deleteAll:     EventEmitter<string> = new EventEmitter<string>();
	onChange(event : any) {
		this.moduleCategorie = Utils.findByName(this.moduleCategories, this.categorieToAdd);
		if(this.moduleCategorie.modules.length > 0) {
			this.moduleToAdd = this.moduleCategorie.modules[0];
		}
	}
	onDownload(event : any) {
		// alert('Project: ' + JSON.stringify(this.project));
		var dataStr = 'data:text/json;charset=utf-8,' + encodeURIComponent(JSON.stringify(this.project, null, '\t'));
		var dlAnchorElem = document.getElementById('downloadAnchorElem');
		dlAnchorElem.setAttribute('href',     dataStr     );
		dlAnchorElem.setAttribute('download', this.project.name + '.json');
		dlAnchorElem.click();
	}
	onUploadProject(event : any) {
		this.uploadProject.emit(event);
	}
	onCreate(event : string) : void {
		this.createModule.emit(this.moduleToAdd);
	}
	onDeleteAll(event : any) {
		this.deleteAll.emit(event);
	}
}

