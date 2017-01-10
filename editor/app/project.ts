import {Module} from './module';

export class Project {
	public name: string;
	public description: string;
	public modules: Array<Module>;
	public static readFromFile(project: any, file: any) {
		let obj = JSON.parse(file);
		project.name        = obj.name;
		project.description = obj.description;
		project.modules     = obj.modules;
	}
}
