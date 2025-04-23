from repo import Repo
from assignment import Assignment
import json

if __name__ == "__main__":
    owner = "bmw-ece-ntust"
    repo_name = "multimedia-wireless-networks"
    
    repo = Repo(owner, repo_name)
    repo.add_branch("2025")
    
    print(repo.branches[1])
    
    for branch in repo.branches:
        print(f"Branch: {branch.name}")
        branch.add_assignments("a1")
