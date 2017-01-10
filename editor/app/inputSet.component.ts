import {
	Component,
	Input,
	ViewEncapsulation
} from '@angular/core';
import {ModuleInput}  from './moduleInput';
import {Utils} from './utils';

@Component({
	selector: 'input-set',
	template: `
<input type='checkbox' (change)='activate($event)' value='input!==undefined'/>
`,
	host: {
	},
	styles: [
`
`
	],
	encapsulation: ViewEncapsulation.Emulated
})
export class InputSet {
	@Input() private input: ModuleInput;
	@Input() private description: any;
	@Input() private inputs: Array<any>;
	activate($event: any): void {
		if($event.target.checked) {
			this.input = Utils.findByNameAndAdd(this.inputs, this.description.name);
			this.input.value = this.description.default;
		} else {
			Utils.removeByName(this.inputs, this.description.name);
			this.input = undefined;
		}
	}
}
