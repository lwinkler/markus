import {
	Component,
	Input,
	ViewEncapsulation
} from '@angular/core';
import {ModuleInput}  from './moduleInput';
import {Utils} from './utils';

@Component({
	selector: 'input-value',
	template: `
<input type='checkbox' (change)='activate($event)' value='input!==undefined'/>
<span *ngIf='input!=undefined'>
	<span type='text' *ngIf='input.connected'>(connected)</span>
	<div [ngSwitch]='description.type' *ngIf='input.value!==undefined'>
		<input *ngSwitchCase='"float"'        min='{{description.range.min}}' max='{{description.range.max}}' step='0.01' [(ngModel)]='input.value' (ngModelChange)='cast()'/>
		<input *ngSwitchCase='"double"'       min='{{description.range.min}}' max='{{description.range.max}}' step='0.01' [(ngModel)]='input.value' (ngModelChange)='cast()'/>
		<input *ngSwitchCase='"int"'          min='{{description.range.min}}' max='{{description.range.max}}' step='1'    [(ngModel)]='input.value' (ngModelChange)='cast()'/>
		<input *ngSwitchCase='"unsigned int"' min='{{description.range.min}}' max='{{description.range.max}}' step='1'    [(ngModel)]='input.value' (ngModelChange)='cast()'/>
		<input *ngSwitchCase='"bool"' type='checkbox' [(ngModel)]='input.value'/>
		<input *ngSwitchCase='"string"' type='string' [(ngModel)]='input.value'/>
		<span *ngSwitchDefault>Unknown type {{description.type}}</span>
	</div>
</span>
<span *ngIf='input===undefined'>
	(unset)
</span>
`,
	styles: [
`.input-value {
}`
	],
	encapsulation: ViewEncapsulation.Emulated
})
export class InputValue {
	@Input()
	input: ModuleInput;
	@Input()
	description: any;
	@Input()
	inputs: Array<any>;
	activate($event: any): void {
		if($event.target.checked) {
			this.input = Utils.findByNameAndAdd(this.inputs, this.description.name);
			this.input.value = this.description.default;
		} else {
			Utils.removeByName(this.inputs, this.description.name);
			this.input = undefined;
		}
	}
	cast(): any { // TODO: split
		switch(this.description.type) {
			case 'float':
			case 'double':
				this.input.value = parseFloat(this.input.value);
				break;
			case 'int':
			case 'unsigned int':
				this.input.value = parseInt(this.input.value, 10);
				break;
			case 'bool':
				this.input.value = this.input.value > 0;
				break;
		}
	}
}
