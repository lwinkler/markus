export class Utils {
	static findByName(list: Array<any>, name: string) {
		for(let val of list) {
			if(val.name === name)
				return val;
		}
		// throw ('No object with name ' + name + ' in list');
		return undefined;
	}
	static findByNameAndAdd(list: Array<any>, name: string) {
		for(let val of list) {
			if(val.name === name)
				return val;
		}
		list.push({name: name});
		return list[list.length - 1];
	}
	static removeByName(list: Array<any>, name: string) {
		let ind = 0;
		for(let val of list) {
			if(val.name === name) {
				list.splice(ind, 1);
				return;
			}
			ind++;
		}
		throw ('No object with name ' + name + ' in list');
	}
	static findByClass(list: Array<any>, name: string) {
		for(let val of list) {
			if(val.class === name)
				return val;
		}
		// throw ('No object with class ' + name + ' in list');
		return undefined;
	}
}
