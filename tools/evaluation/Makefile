protect:
	@echo ">>> Precise a target"

clean:
	@rm out_latest -rf
	@echo -e "\e[91mThe following files will be removed:\e[0m"
	@find . -name *.pyc
	@echo -en "\e[91mAre you sure? [y/n]\e[0m "
	@read -n 1 -r REPLY; \
	echo ''; \
	if [[ $${REPLY} =~ ^[Yy]$$ ]]; then \
		find . -name *.pyc -exec rm {} \; ;\
	else \
		echo -e "\e[93mAborting…\e[0m"; \
	fi

destroy:
	@echo -e "\e[91mThe following results will be destroyed:\e[0m"
	@git clean -n -d -x .
	@echo -en "\e[91mAre you sure? [y/n/i]\e[0m "
	@read -n 1 -r REPLY; \
	echo ''; \
	if [[ $${REPLY} =~ ^[Ii]$$ ]]; then \
		git clean -d -x -i; \
	elif [[ $${REPLY} =~ ^[Yy]$$ ]]; then \
		git clean -d -x -f; \
	else \
		echo -e "\e[93mAborting…\e[0m"; \
	fi
