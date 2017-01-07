import {Module} from './module';

export class Project {
	name: string;
	description: string;
	modules: Array<Module>;
	static readFromFile(project : any, file : any) {
		let obj = JSON.parse(file);
		project.name        = obj.name;
		project.description = obj.description;
		project.modules     = obj.modules;
	}
}
