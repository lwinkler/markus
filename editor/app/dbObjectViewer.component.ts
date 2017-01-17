import {Component} from '@angular/core';
import {Job} from './job';
import {Http, Response} from '@angular/http';
import {Utils} from './utils';

declare var g_controller: any;

@Component({
	selector: 'db-object-viewer',
	template: `
<div class='db-object-viewer'>
	<h1>Database</h1>
	Select db: <select (change)='onChangeDb($event)' [(ngModel)]='selectedDb'>
		<option *ngFor='let db of dbs' [value]='db'>{{db}}</option>
	</select>
	<h1>Job</h1>
	Select job: <select (change)='onChangeJob($event)'>
		<option *ngFor='let job of jobs' [value]='job._id'>{{job._id}} {{job.applicationName}} {{job.startDate}}</option>
	</select>

	<h1>Objects</h1>
	<table class='object-table'>
		<tr><th>Id</th><th>name</th></tr>
		<tr *ngFor='let object of objects'>
			<td>{{object._id}}</td>
			<td>{{object.name}}</td>
		</tr>
		<tr *ngIf='objects.length==0'><td>-</td><td>(empty)</td></tr>
	</table>
</div>
`
})
export class DbObjectViewer {
	private dbs: Array<Job> = [];
	private jobs: Array<Job> = [];
	private objects: Array<Object> = [];
	private selectedDb: string;
	// private selectedJob: Job;
	private selectedObject: Object;

	
	constructor(public http: Http) {
		http.get('http://localhost:3000/dbs').map((res: Response) => res.json()).subscribe(res => this.dbs = res);
	}

	onChangeDb($event: any) {
		this.http.get('http://localhost:3000/' + $event.target.value + '/jobs').map((res: Response) => res.json()).subscribe(res => this.jobs = res);
	}

	onChangeJob($event: any) {
		this.http.get('http://localhost:3000/' + this.selectedDb + '/objects?query={"jobId":"' + $event.target.value + '"}').map((res: Response) => res.json()).subscribe(res => this.objects = res);
	}

}
